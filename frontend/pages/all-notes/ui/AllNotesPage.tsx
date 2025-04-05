import { useState, useEffect } from "react";
import { getAllNotes } from "../api/notes";
import type { NoteResponse } from "shared/api";
import { Sidebar } from "widgets/sidebar";
import { NoteListItem } from "entities/note";
import { Pagination } from "./Pagination";

export function AllNotesPage() {
  const [notes, setNotes] = useState<NoteResponse[]>([]);
  const [totalCount, setTotalCount] = useState<number>(0);
  const [page, setPage] = useState<number>(1);
  const [limit] = useState<number>(20);
  const [loading, setLoading] = useState<boolean>(true);
  const [error, setError] = useState<string>("");

  const totalPages = Math.ceil(totalCount / limit);

  useEffect(() => {
    async function fetchNotes() {
      try {
        setLoading(true);
        const data = await getAllNotes(page, limit);
        setNotes(data.notes);
        setTotalCount(data.total_count);
      } catch (err) {
        setError("Error fetching notes.");
      } finally {
        setLoading(false);
      }
    }
    fetchNotes();
  }, [page, limit]);

  if (loading) return <div>Loading...</div>;
  if (error) return <div>{error}</div>;

  return (
    <div className="flex h-screen">
      <Sidebar />
      <div className="p-6 w-full">
        <h1 className="text-center text-white drop-shadow text-2xl font-bold mb-6">All Notes</h1>
        
        <Pagination page={page} totalPages={totalPages} onPageChange={setPage} />
        
        <ul className="space-y-4">
          {notes.map((note) => (
            <NoteListItem key={note.note_id} note={note} />
          ))}
        </ul>
        
        <Pagination page={page} totalPages={totalPages} onPageChange={setPage} />
      </div>
    </div>
  );
}
