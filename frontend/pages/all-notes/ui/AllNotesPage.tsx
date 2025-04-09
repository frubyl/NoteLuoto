import { useState, useEffect, memo } from "react";
import { getAllNotes } from "../api/notes";
import type { NoteResponse } from "shared/api";
import { Sidebar } from "widgets/sidebar";
import { NoteListItem } from "entities/note";
import { Pagination } from "./Pagination";

const SearchBar = memo(function SearchBar({
  tempSearchQuery,
  setTempSearchQuery,
  searchType,
  onSearchTypeChange,
}: {
  tempSearchQuery: string;
  setTempSearchQuery: (value: string) => void;
  searchType: "semantic" | "exact" | "ai";
  onSearchTypeChange: (type: "semantic" | "exact" | "ai") => void;
}) {
  return (
    <div className="mb-6">
      <div className="flex space-x-2 mb-2">
        <button
          className={`px-4 py-2 rounded ${
            searchType === "semantic" ? "bg-blue-500 text-white" : "bg-gray-200"
          }`}
          onClick={() => onSearchTypeChange("semantic")}
        >
          Semantic Search
        </button>
        <button
          className={`px-4 py-2 rounded ${
            searchType === "exact" ? "bg-blue-500 text-white" : "bg-gray-200"
          }`}
          onClick={() => onSearchTypeChange("exact")}
        >
          Exact Search
        </button>
        <button
          className={`px-4 py-2 rounded ${
            searchType === "ai" ? "bg-blue-500 text-white" : "bg-gray-200"
          }`}
          onClick={() => onSearchTypeChange("ai")}
        >
          AI Search
        </button>
      </div>
      <input
        type="text"
        placeholder="Search..."
        value={tempSearchQuery}
        onChange={(e) => setTempSearchQuery(e.target.value)}
        className="w-full px-4 py-2 rounded"
      />
    </div>
  );
});

export function AllNotesPage() {
  const [notes, setNotes] = useState<NoteResponse[]>([]);
  const [totalCount, setTotalCount] = useState<number>(0);
  const [page, setPage] = useState<number>(1);
  const [limit] = useState<number>(20);
  const [loading, setLoading] = useState<boolean>(true);
  const [error, setError] = useState<string>("");
  const [tempSearchQuery, setTempSearchQuery] = useState<string>("");
  const [searchQuery, setSearchQuery] = useState<string>("");
  const [searchType, setSearchType] = useState<"semantic" | "exact" | "ai">("semantic");

  const totalPages = Math.ceil(totalCount / limit);

  useEffect(() => {
    const timer = setTimeout(() => {
      setSearchQuery(tempSearchQuery);
      setPage(1);
    }, 1500);
    return () => clearTimeout(timer);
  }, [tempSearchQuery]);

  useEffect(() => {
    async function fetchNotes() {
      try {
        setLoading(true);
        const data = await getAllNotes(page, limit, searchQuery, searchType);
        setNotes(data.notes);
        setTotalCount(data.total_count);
      } catch (err) {
        setError("Error fetching notes.");
      } finally {
        setLoading(false);
      }
    }
    fetchNotes();
  }, [page, limit, searchQuery, searchType]);

  const handleSearchTypeChange = (type: "semantic" | "exact" | "ai") => {
    setSearchType(type);
    setPage(1);
  };

  if (loading) return <div>Loading...</div>;
  if (error) return <div>{error}</div>;

  return (
    <div className="flex h-screen">
      <Sidebar />
      <div className="p-6 w-full">
        <h1 className="text-center text-white drop-shadow text-2xl font-bold mb-6">All Notes</h1>

        <SearchBar
          tempSearchQuery={tempSearchQuery}
          setTempSearchQuery={setTempSearchQuery}
          searchType={searchType}
          onSearchTypeChange={handleSearchTypeChange}
        />

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
