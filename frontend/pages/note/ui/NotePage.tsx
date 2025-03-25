import { useEffect, useState } from 'react';
import { useParams, useNavigate } from 'react-router';
import { getNote, updateNote, deleteNote } from "../api/note";
import type { NotePatchRequest } from "shared/api";
import { Sidebar } from 'widgets/sidebar';
import { TagModal } from './TagModal';
import { 
  getTagsForNote, 
  removeTagFromNote, 
  type Tag 
} from "../api/tag";

export function NotePage() {
  const { note_id } = useParams();
  const navigate = useNavigate();
  const [note, setNote] = useState<NotePatchRequest | null>(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState("");
  const [editing, setEditing] = useState(false);
  const [formData, setFormData] = useState<NotePatchRequest>({ title: "", body: "" });
  const [noteTags, setNoteTags] = useState<Tag[]>([]);
  const [tagModalOpen, setTagModalOpen] = useState(false);

  useEffect(() => {
    async function fetchNote() {
      try {
        if (note_id) {
          const data = await getNote(parseInt(note_id));
          setNote(data);
          setFormData({ title: data.title, body: data.body });
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
  }, [note_id]);

  const handleSave = async () => {
    try {
      if (note_id) {
        await updateNote(parseInt(note_id), formData);
        setEditing(false);
        const updatedNote = await getNote(parseInt(note_id));
        setNote(updatedNote);
      }
    } catch (err) {
      setError("Error updating note.");
    }
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
      <div className="p-6 w-full">
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

        {editing ? (
          <div>
            <input
              type="text"
              value={formData.title}
              onChange={(e) =>
                setFormData({ ...formData, title: e.target.value })
              }
              placeholder="Title"
              className="mb-4 p-2 w-full"
            />
            <textarea
              value={formData.body}
              onChange={(e) =>
                setFormData({ ...formData, body: e.target.value })
              }
              placeholder="Note body"
              className="mb-4 p-2 w-full h-64"
            />
            <button onClick={handleSave} className="mr-2 p-2 bg-green-500 text-white rounded">
              Save
            </button>
            <button onClick={() => setEditing(false)} className="p-2 bg-gray-500 text-white rounded">
              Cancel
            </button>
          </div>
        ) : (
          <div>
            <h1 className="text-white text-2xl font-bold mb-4">{note.title}</h1>
            <p className="text-white mb-6 whitespace-pre-wrap">{note.body}</p>
            <div>
              <button onClick={() => setEditing(true)} className="mr-2 p-2 bg-blue-500 text-white rounded">
                Edit
              </button>
              <button onClick={handleDelete} className="p-2 bg-red-500 text-white rounded">
                Delete
              </button>
            </div>
          </div>
        )}
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
