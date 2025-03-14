import { useState, useEffect } from 'react';
import { getRecentNotes } from "../api/home";
import type { NoteResponse } from "shared/api";
import { Sidebar } from 'widgets/sidebar';
import { Link } from 'react-router';

export function HomePage() {
  const [notes, setNotes] = useState<NoteResponse[]>([]);
  const [loading, setLoading] = useState<boolean>(true);
  const [error, setError] = useState<string>("");

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

  if (loading) return <div>Loading...</div>;
  if (error) return <div>{error}</div>;

  return (
    <div className="flex h-screen">
      <Sidebar />
      <div className="p-6 w-full">
        <h1 className="text-white drop-shadow text-2xl font-bold mb-6">Recent Notes</h1>
        <ul className="space-y-4">
          {notes.map((note) => (
            <li key={note.note_id}>
            <Link
              to={`/notes/${note.note_id}`}
              className="block p-4 bg-black rounded-lg shadow hover:shadow-md transition duration-200 hover:bg-[#1a1a1a]"
            >
              <div className="flex justify-between items-center">
                <h2 className="text-[#ccc2dc] text-xl font-semibold">{note.title}</h2>
                <span className="text-sm text-[#ccc2dc]">
                  {new Date(note.created_at).toLocaleString()}
                </span>
              </div>
            </Link>
          </li>
          ))}
        </ul>
      </div>
    </div>
  );
}
