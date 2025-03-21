import { useEffect, useState } from 'react';
import { useParams, useNavigate } from 'react-router';
import { getNote, updateNote, deleteNote } from "../api/note";
import type { NotePatchRequest } from "shared/api";
import { Sidebar } from 'widgets/sidebar';

export function NotePage() {
  const { note_id } = useParams();
  const navigate = useNavigate();
  const [note, setNote] = useState<NotePatchRequest | null>(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState("");
  const [editing, setEditing] = useState(false);
  const [formData, setFormData] = useState<NotePatchRequest>({ title: "", body: "" });

  useEffect(() => {
    async function fetchNote() {
      try {
        if (note_id) {
          const data = await getNote(parseInt(note_id));
          setNote(data);
          setFormData({ title: data.title, body: data.body });
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

  if (loading) return <div>Loading...</div>;
  if (error) return <div>{error}</div>;
  if (!note) return <div>No note found</div>;

  return (
    <div className="flex h-screen">
      <Sidebar />
      <div className="p-6 w-full">
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
    </div>
  );
}
