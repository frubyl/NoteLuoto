import { useState, useEffect } from 'react';
import { getRecentNotes, createNote } from "../api/home";
import type { NoteResponse } from "shared/api";
import { Sidebar } from 'widgets/sidebar';
import { useNavigate } from 'react-router';
import { NoteListItem } from 'entities/note';

export function HomePage() {
  const [notes, setNotes] = useState<NoteResponse[]>([]);
  const [loading, setLoading] = useState<boolean>(true);
  const [error, setError] = useState<string>("");
  const [newNoteTitle, setNewNoteTitle] = useState<string>("");
  const [creating, setCreating] = useState<boolean>(false);
  const navigate = useNavigate();

  useEffect(() => {
    async function fetchNotes() {
      try {
        const { status, notes } = await getRecentNotes(10);
        if (status === 200 && notes) {
          setNotes(notes);
        } else {
          setError("Error fetching notes.");
        }
      } catch (err) {
        setError("An unexpected error occurred.");
      } finally {
        setLoading(false);
      }
    }
    fetchNotes();
  }, []);

  const handleCreateNote = async () => {
    if (!newNoteTitle.trim()) {
      setError("Please enter a note title.");
      return;
    }

    setCreating(true);
    try {
      const noteId = await createNote({ title: newNoteTitle, body: "" });
      navigate(`/notes/${noteId}`);
    } catch (err) {
      setError("Error creating note.");
    } finally {
      setCreating(false);
    }
  };

  if (loading) return <div>Loading...</div>;
  if (error) return <div>{error}</div>;

  return (
    <div className="flex h-screen">
      <Sidebar />
      <div className="p-6 w-full">
        <input
          type="text"
          placeholder="Enter note title..."
          value={newNoteTitle}
          onChange={(e) => setNewNoteTitle(e.target.value)}
          className="p-1 mb-2 rounded border border-gray-300 w-full"
        />
        <button
          onClick={handleCreateNote}
          disabled={creating}
          className="p-2 justify-center rounded-md text-center block bg-black text-[#ccc2dc] text-sm rounded-lg shadow hover:shadow-md hover:bg-[#1a1a1a] transition"
        >
          {creating ? "Creating..." : "Create Note"}
        </button>
        <h1 className="text-center text-white drop-shadow text-2xl font-bold mb-6">Recent Notes</h1>

        <ul className="space-y-4">
          {notes.map(note => (
            <NoteListItem key={note.note_id} note={note} />
          ))}
        </ul>
      </div>
    </div>
  );
}
