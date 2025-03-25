import { useState, useEffect } from "react";
import { getAllNotes } from "../api/notes";
import type { NoteResponse } from "shared/api";
import { Sidebar } from "widgets/sidebar";
import { NoteListItem } from "entities/note";

export function AllNotesPage() {
  const [notes, setNotes] = useState<NoteResponse[]>([]);
  const [page, setPage] = useState<number>(1);
  const [limit] = useState<number>(20);
  const [loading, setLoading] = useState<boolean>(true);
  const [error, setError] = useState<string>("");
  const [hasNextPage, setHasNextPage] = useState<boolean>(true);

  useEffect(() => {
    async function fetchNotes() {
      try {
        setLoading(true);
        const data = await getAllNotes(page, limit);
        setNotes(data);
        setHasNextPage(data.length === limit);
      } catch (err) {
        setError("Error fetching notes.");
      } finally {
        setLoading(false);
      }
    }
    fetchNotes();
  }, [page, limit]);

  const handlePrevious = () => {
    if (page > 1) {
      setPage(page - 1);
    }
  };

  const handleNext = () => {
    if (hasNextPage) {
      setPage(page + 1);
    }
  };

  if (loading) return <div>Loading...</div>;
  if (error) return <div>{error}</div>;

  return (
    <div className="flex h-screen">
      <Sidebar />
      <div className="p-6 w-full">
        <h1 className="text-center text-white drop-shadow text-2xl font-bold mb-6">All Notes</h1>
        <ul className="space-y-4">
          {notes.map(note => (
            <NoteListItem key={note.note_id} note={note} />
          ))}
        </ul>
        <div className="flex justify-center items-center mt-6 space-x-4">
          <button
            onClick={handlePrevious}
            disabled={page === 1}
            className="px-4 py-2 bg-gray-700 text-white rounded disabled:opacity-50"
          >
            Previous
          </button>
          <span className="text-white">Page {page}</span>
          <button
            onClick={handleNext}
            disabled={!hasNextPage}
            className="px-4 py-2 bg-gray-700 text-white rounded disabled:opacity-50"
          >
            Next
          </button>
        </div>
      </div>
    </div>
  );
}
