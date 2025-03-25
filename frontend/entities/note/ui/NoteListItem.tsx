import { Link } from "react-router";
import type { NoteResponse } from "shared/api";

type NoteListItemProps = {
  note: NoteResponse;
};

export function NoteListItem({ note }: NoteListItemProps) {
  return (
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
  );
}
