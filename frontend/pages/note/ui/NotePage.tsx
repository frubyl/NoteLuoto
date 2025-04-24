import { useEffect, useRef, useState } from 'react';
import { useParams, useNavigate } from 'react-router';
import { getNote, updateNote, deleteNote } from "../api/note";
import type { NotePatchRequest } from "shared/api";
import { Sidebar } from 'widgets/sidebar';
import { TagModal } from './TagModal';
import Highlight from '@tiptap/extension-highlight';
import Typography from '@tiptap/extension-typography';
import Image from '@tiptap/extension-image';
import { EditorContent, useEditor } from '@tiptap/react';
import StarterKit from '@tiptap/starter-kit';
import {
  getTagsForNote,
  removeTagFromNote,
  suggestTags,
  createTag,
  addTagToNote,
  type Tag,
} from "../api/tag";
import { marked } from 'marked';
import TurndownService from 'turndown';
import {
  getChecklistsForNote,
  createChecklist,
  getChecklistById,
  updateChecklistTitle,
  deleteChecklist,
  addChecklistItem,
  updateChecklistItem,
  deleteChecklistItem,
  type Checklist,
  type ChecklistItem,
} from '../api/checklist';
import {
  addAttachmentToNote,
  getAttachmentsForNote,
  getAttachmentFromNote,
  deleteAttachmentFromNote,
  type AttachmentReference,
  type AttachmentData,
} from "../api/attachment";

const turndownService = new TurndownService({ headingStyle: 'atx' });

export function NotePage() {
  const { note_id } = useParams<{ note_id: string }>();
  const navigate = useNavigate();

  const [note, setNote] = useState<NotePatchRequest | null>(null);
  const [formData, setFormData] = useState<NotePatchRequest>({ title: "", body: "" });
  const editor = useEditor({
    extensions: [StarterKit, Highlight, Typography, Image],
    content: '',
  });
  const saveTimer = useRef<NodeJS.Timeout | null>(null);
  const scheduleAutoSave = () => {
    if (saveTimer.current) clearTimeout(saveTimer.current);
    saveTimer.current = setTimeout(autoSave, 2000);
  };
  async function autoSave() {
    if (!note_id || !editor) return;
    const md = turndownService.turndown(editor.getHTML());
    await updateNote(parseInt(note_id), { title: formData.title, body: md });
    setNote(await getNote(parseInt(note_id)));
  }

  const [noteTags, setNoteTags] = useState<Tag[]>([]);
  const [tagModalOpen, setTagModalOpen] = useState(false);
  const [suggestedTags, setSuggestedTags] = useState<string[]>([]);
  const [loadingSuggest, setLoadingSuggest] = useState(false);

  const [checklists, setChecklists] = useState<Checklist[]>([]);
  const [newChecklistTitle, setNewChecklistTitle] = useState('');
  const [editingChecklistId, setEditingChecklistId] = useState<number | null>(null);
  const [editedTitles, setEditedTitles] = useState<Record<number, string>>({});
  const [newItemTexts, setNewItemTexts] = useState<Record<number, string>>({});

  const [attachments, setAttachments] = useState<AttachmentReference[]>([]);
  const fileInputRef = useRef<HTMLInputElement>(null);

  const [loading, setLoading] = useState(true);
  const [error, setError] = useState("");

  useEffect(() => {
    async function fetchAll() {
      if (!note_id) return;
      try {
        const n = await getNote(parseInt(note_id));
        setNote(n);
        setFormData({ title: n.title, body: n.body });
        if (editor) editor.commands.setContent(marked(n.body ?? ''));
        setNoteTags(await getTagsForNote(parseInt(note_id)));
        setChecklists(await getChecklistsForNote(parseInt(note_id)));
        setAttachments(await getAttachmentsForNote(parseInt(note_id)));
      } catch {
        setError("Error fetching note.");
      } finally {
        setLoading(false);
      }
    }
    fetchAll();
  }, [note_id, editor]);

  useEffect(() => {
    if (!editor) return;
    const fn = () => scheduleAutoSave();
    editor.on('update', fn);
    return () => {
      editor.off('update', fn);
      if (saveTimer.current) clearTimeout(saveTimer.current);
    };
  }, [editor, formData.title]);

  useEffect(() => {
    if (editingChecklistId !== null) {
      const cl = checklists.find(c => c.checklist_id === editingChecklistId);
      setEditedTitles(et => ({ ...et, [editingChecklistId]: cl?.title || '' }));
    }
  }, [checklists, editingChecklistId]);

  const handleTitleChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    setFormData(fd => ({ ...fd, title: e.target.value }));
    scheduleAutoSave();
  };
  const handleDeleteNote = async () => {
    if (!note_id) return;
    await deleteNote(parseInt(note_id));
    navigate('/');
  };
  const handleRemoveTag = async (id: number) => {
    if (!note_id) return;
    await removeTagFromNote(parseInt(note_id), id);
    setNoteTags(await getTagsForNote(parseInt(note_id)));
  };
  const handleSuggestTags = async () => {
    if (!note_id) return;
    setLoadingSuggest(true);
    const s = (await suggestTags(parseInt(note_id))).filter(t => !noteTags.some(nt => nt.name === t));
    setSuggestedTags(s);
    setLoadingSuggest(false);
  };
  const handleAddRecommendedTag = async (name: string) => {
    if (!note_id) return;
    const t = await createTag(name);
    await addTagToNote(parseInt(note_id), t.tag_id);
    setNoteTags(await getTagsForNote(parseInt(note_id)));
    setSuggestedTags(st => st.filter(x => x !== name));
  };

  const refreshChecklists = async () => {
    if (!note_id) return;
    setChecklists(await getChecklistsForNote(parseInt(note_id)));
  };

  const handleCreateChecklist = async () => {
    if (!note_id || !newChecklistTitle.trim()) return;
    await createChecklist(parseInt(note_id), newChecklistTitle.trim());
    setNewChecklistTitle('');
    await refreshChecklists();
  };

  const handleStartEditChecklistTitle = (id: number) => {
    setEditingChecklistId(id);
  };
  const handleCancelEditChecklistTitle = () => {
    setEditingChecklistId(null);
  };
  const handleSaveChecklistTitle = async (id: number) => {
    const title = editedTitles[id]?.trim();
    if (!title) return;
    await updateChecklistTitle(id, title);
    setEditingChecklistId(null);
    await refreshChecklists();
  };

  const handleDeleteChecklist = async (id: number) => {
    await deleteChecklist(id);
    setChecklists(cl => cl.filter(c => c.checklist_id !== id));
  };

  const handleAddItem = async (checklistId: number) => {
    const text = newItemTexts[checklistId]?.trim();
    if (!text) return;
    await addChecklistItem(checklistId, text);
    setNewItemTexts(nt => ({ ...nt, [checklistId]: '' }));
    await refreshChecklists();
  };

  const handleToggleItem = async (checklistId: number, it: ChecklistItem) => {
    await updateChecklistItem(it.item_id, { status: !it.completed });
    await refreshChecklists();
  };

  const handleDeleteItem = async (it: ChecklistItem) => {
    await deleteChecklistItem(it.item_id);
    await refreshChecklists();
  };

  const refreshAttachments = async () => {
    if (note_id) return setAttachments(await getAttachmentsForNote(parseInt(note_id)));
  };
  const handleUpload = async () => {
    if (!note_id || !fileInputRef.current?.files?.[0]) return;
    await addAttachmentToNote(parseInt(note_id), fileInputRef.current.files![0]);
    fileInputRef.current.value = "";
    await refreshAttachments();
  };
  const handleDownload = async (attId: number) => {
    const { file_name, content } = await getAttachmentFromNote(attId);
    const byteString = content
    const ab = new ArrayBuffer(byteString.length);
    const ia = new Uint8Array(ab);
    for (let i = 0; i < byteString.length; i++) ia[i] = byteString.charCodeAt(i);
    const blob = new Blob([ab]);
    const url = URL.createObjectURL(blob);
    const a = document.createElement("a");
    a.href = url;
    a.download = file_name;
    a.click();
    URL.revokeObjectURL(url);
  };
  const handleDeleteAttachment = async (attId: number) => {
    await deleteAttachmentFromNote(attId);
    await refreshAttachments();
  };

  if (loading) return <div>Loading...</div>;
  if (error) return <div className="p-4 text-red-600">{error}</div>;
  if (!note) return <div>No note found</div>;

  return (
    <div className="flex h-screen">
      <Sidebar />
      <div className="p-6 w-full overflow-y-auto">

        <div className="mb-4">
          <div className="flex items-center space-x-2 mb-2">
            <button onClick={() => setTagModalOpen(true)}
              className="bg-green-500 text-white px-3 py-1 rounded">
              Add Tag
            </button>
            {noteTags.map(tag => (
              <div key={tag.tag_id}
                className="bg-gray-200 text-gray-800 px-3 py-1 rounded flex items-center">
                <span>{tag.name}</span>
                <button onClick={() => handleRemoveTag(tag.tag_id)}
                  className="ml-2 text-red-500 font-bold">×
                </button>
              </div>
            ))}
          </div>
          <button onClick={handleSuggestTags}
            disabled={loadingSuggest}
            className={`px-3 py-1 rounded ${loadingSuggest ? 'bg-gray-400' : 'bg-blue-500'} text-white`}>
            {loadingSuggest ? "Loading..." : "Suggest Tags"}
          </button>
          <div className="mt-2 flex flex-wrap gap-2">
            {suggestedTags.map((t, i) => (
              <button key={i}
                onClick={() => handleAddRecommendedTag(t)}
                className="px-2 py-1 bg-blue-200 text-blue-800 rounded hover:bg-blue-300">
                {t}
              </button>
            ))}
          </div>
        </div>

        <input type="text"
          value={formData.title}
          onChange={handleTitleChange}
          placeholder="Title"
          className="mb-4 p-2 w-full border rounded" />
        <EditorContent className="bg-white p-2 border rounded mb-4" editor={editor} />

        <div className="flex space-x-2 mb-6">
          <button onClick={handleDeleteNote}
            className="p-2 bg-red-500 text-white rounded">
            Delete Note
          </button>
        </div>

        <div className="border-t pt-6">
          <h3 className="text-lg font-bold mb-4">Checklists</h3>

          <div className="mb-6 flex gap-2">
            <input type="text"
              placeholder="New checklist title"
              value={newChecklistTitle}
              onChange={e => setNewChecklistTitle(e.target.value)}
              className="flex-1 p-2 border rounded" />
            <button onClick={handleCreateChecklist}
              className="bg-blue-500 text-white px-4 py-2 rounded">
              Create
            </button>
          </div>

          {checklists.map(cl => (
            <div key={cl.checklist_id} className="mb-8 border p-4 rounded shadow-sm bg-white">
              <div className="flex justify-between items-center mb-2">
                {editingChecklistId === cl.checklist_id ? (
                  <div className="flex gap-2 w-full">
                    <input value={editedTitles[cl.checklist_id] || ''}
                      onChange={e => setEditedTitles(et => ({
                        ...et,
                        [cl.checklist_id]: e.target.value
                      }))}
                      className="flex-1 p-2 border rounded" />
                    <button onClick={() => handleSaveChecklistTitle(cl.checklist_id)}
                      className="px-3 py-1 bg-green-500 text-white rounded">
                      Save
                    </button>
                    <button onClick={handleCancelEditChecklistTitle}
                      className="px-3 py-1 bg-gray-300 rounded">
                      Cancel
                    </button>
                  </div>
                ) : (
                  <>
                    <h4 className="text-md font-semibold">{cl.title}</h4>
                    <div className="flex gap-2">
                      <button onClick={() => handleStartEditChecklistTitle(cl.checklist_id)}
                        className="text-blue-500 hover:underline">
                        Edit
                      </button>
                      <button onClick={() => handleDeleteChecklist(cl.checklist_id)}
                        className="text-red-500 hover:underline">
                        Delete
                      </button>
                    </div>
                  </>
                )}
              </div>

              <ul className="space-y-2 mb-4">
                {cl.items.map(it => (
                  <li key={it.item_id} className="flex items-center space-x-2">
                    <input type="checkbox"
                      checked={it.completed}
                      onChange={() => handleToggleItem(cl.checklist_id, it)} />
                    <span className={it.completed ? 'line-through text-gray-500' : ''}>
                      {it.text}
                    </span>
                    <button onClick={() => handleDeleteItem(it)}
                      className="ml-auto text-red-500 font-bold">
                      ×
                    </button>
                  </li>
                ))}
              </ul>

              <div className="flex gap-2">
                <input type="text"
                  placeholder="New item..."
                  value={newItemTexts[cl.checklist_id] || ''}
                  onChange={e => setNewItemTexts(nt => ({
                    ...nt,
                    [cl.checklist_id]: e.target.value
                  }))}
                  className="flex-1 p-2 border rounded" />
                <button onClick={() => handleAddItem(cl.checklist_id)}
                  className="bg-green-500 text-white px-4 py-2 rounded">
                  Add
                </button>
              </div>
            </div>
          ))}
        </div>
        <div className="border-t pt-6">
          <h3 className="text-lg font-bold mb-4">Attachments</h3>
          <div className="mb-4 flex gap-2 items-center">
            <input type="file" ref={fileInputRef} />
            <button
              onClick={handleUpload}
              className="bg-blue-500 text-white px-4 py-2 rounded"
            >
              Upload
            </button>
          </div>
          <ul className="space-y-2">
            {attachments.map((att) => (
              <li key={att.attachment_id} className="flex items-center space-x-2">
                <span>Attachment #{att.attachment_id}</span>
                <button
                  onClick={() => handleDownload(att.attachment_id)}
                  className="text-blue-500 hover:underline"
                >
                  Download
                </button>
                <button
                  onClick={() => handleDeleteAttachment(att.attachment_id)}
                  className="text-red-500 hover:underline"
                >
                  Delete
                </button>
              </li>
            ))}
          </ul>
        </div>
      </div>

      {tagModalOpen && note_id && (
        <TagModal
          noteId={parseInt(note_id)}
          onClose={() => setTagModalOpen(false)}
          onTagAdded={async () => setNoteTags(await getTagsForNote(parseInt(note_id)))}
        />
      )}
    </div>
  );
}
