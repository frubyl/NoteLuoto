import { GET, POST } from "shared/api";
import type { NoteCreateRequest, NoteResponse } from "shared/api";

export type RecentNotes = {
  status: number,
  notes: NoteResponse[] | undefined
}

export async function getRecentNotes(num: number): Promise<RecentNotes> {
  const { data, response } = await GET("/notes", { params: { query: { page: 1, limit: num }}})
  
  return {
    status: response.status,
    notes: data?.notes
  }
}

// Create a new note
export async function createNote(payload: NoteCreateRequest): Promise<number> {
  const { data, response } = await POST("/notes", { body: payload });
  if (response.status !== 201 || data?.note_id === undefined) {
    throw new Error("Failed to create note");
  }
  return data.note_id;
}
