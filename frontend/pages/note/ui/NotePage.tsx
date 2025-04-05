import { useEffect, useRef, useState } from 'react';
import { useParams, useNavigate } from 'react-router';
import { getNote, updateNote, deleteNote } from "../api/note";
import type { NotePatchRequest } from "shared/api";
import { Sidebar } from 'widgets/sidebar';
import { TagModal } from './TagModal';
import Highlight from '@tiptap/extension-highlight';
import Typography from '@tiptap/extension-typography';
import Image from '@tiptap/extension-image'
import { EditorContent, useEditor } from '@tiptap/react';
import StarterKit from '@tiptap/starter-kit';
import { getTagsForNote, removeTagFromNote, type Tag } from "../api/tag";
import { marked } from 'marked';
import TurndownService from 'turndown';

const turndownService = new TurndownService({headingStyle: 'atx'});

export function NotePage() {
  const { note_id } = useParams();
  const navigate = useNavigate();
  const [note, setNote] = useState<NotePatchRequest | null>(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState("");
  const [formData, setFormData] = useState<NotePatchRequest>({ title: "", body: "" });
  const [noteTags, setNoteTags] = useState<Tag[]>([]);
  const [tagModalOpen, setTagModalOpen] = useState(false);

  const saveTimer = useRef<NodeJS.Timeout | null>(null);

  const editor = useEditor({
    extensions: [StarterKit, Highlight, Typography, Image],
    content: '',
  });

  const autoSave = async () => {
    if (note_id && editor) {
      const htmlContent = editor.getHTML();
      const markdown = turndownService.turndown(htmlContent);
      try {
        await updateNote(parseInt(note_id), { title: formData.title, body: markdown });
        const updatedNote = await getNote(parseInt(note_id));
        setNote(updatedNote);
      } catch (err) {
        setError("Error updating note.");
      }
    }
  };

  const scheduleAutoSave = () => {
    if (saveTimer.current) {
      clearTimeout(saveTimer.current);
    }
    saveTimer.current = setTimeout(autoSave, 2000);
  };

  useEffect(() => {
    async function fetchNote() {
      try {
        if (note_id) {
          const data = await getNote(parseInt(note_id));
          setNote(data);
          setFormData({ title: data.title, body: data.body });
          if (editor) {
            const html = marked(data.body ?? '');
            editor.commands.setContent(html);
          }
          const tags = await getTagsForNote(parseInt(note_id));
          setNoteTags(tags);
        }
      } catch (err) {
        setError("Error fetching note.");
      } finally {
        setLoading(false);
      }
    }
    fetchNote();
  }, [note_id, editor]);

  useEffect(() => {
    if (editor) {
      const onUpdate = () => {
        scheduleAutoSave();
      };
      editor.on('update', onUpdate);
      return () => {
        editor.off('update', onUpdate);
        if (saveTimer.current) clearTimeout(saveTimer.current);
      };
    }
  }, [editor, formData.title]);

  const handleTitleChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    console.log('a')
    setFormData((prev) => ({ ...prev, title: e.target.value }));
    scheduleAutoSave();
  };

  const handleDelete = async () => {
    try {
      if (note_id) {
        await deleteNote(parseInt(note_id));
        navigate("/");
      }
    } catch (err) {
      setError("Error deleting note.");
    }
  };

  const handleRemoveTag = async (tagId: number) => {
    try {
      if (note_id) {
        await removeTagFromNote(parseInt(note_id), tagId);
        const tags = await getTagsForNote(parseInt(note_id));
        setNoteTags(tags);
      }
    } catch (err) {
      setError("Error removing tag.");
    }
  };

  if (loading) return <div>Loading...</div>;
  if (error) return <div>{error}</div>;
  if (!note) return <div>No note found</div>;

  return (
    <div className="flex h-screen">
      <Sidebar />
      <div className="p-6 w-full overflow-y-auto">
        <div className="mb-4 flex items-center space-x-2">
          <button
            onClick={() => setTagModalOpen(true)}
            className="bg-green-500 text-white px-3 py-1 rounded"
          >
            Add Tag
          </button>
          {noteTags.map(tag => (
            <div key={tag.tag_id} className="bg-gray-200 text-gray-800 px-3 py-1 rounded flex items-center">
              <span>{tag.name}</span>
              <button
                onClick={() => handleRemoveTag(tag.tag_id)}
                className="ml-2 text-red-500 font-bold"
              >
                ×
              </button>
            </div>
          ))}
        </div>

        <input
          type="text"
          value={formData.title}
          onChange={handleTitleChange}
          placeholder="Title"
          className="mb-4 p-2 w-full"
        />

        <EditorContent className="bg-white p-2 border rounded mb-4" editor={editor} />

        <div className="flex space-x-2">
          <button onClick={handleDelete} className="p-2 bg-red-500 text-white rounded">
            Delete
          </button>
        </div>
      </div>
      {tagModalOpen && note_id && (
        <TagModal
          noteId={parseInt(note_id)}
          onClose={() => setTagModalOpen(false)}
          onTagAdded={async () => {
            const tags = await getTagsForNote(parseInt(note_id));
            setNoteTags(tags);
          }}
        />
      )}
    </div>
  );
}
