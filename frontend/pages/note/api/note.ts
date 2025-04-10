import { GET, PATCH, DELETE, POST } from "shared/api";
import type { NoteCreateRequest, NotePatchRequest } from "shared/api";

export async function getNote(noteId: number): Promise<NotePatchRequest> {
  const { data, response } = await GET("/notes/{note_id}", { params: { path: { note_id: noteId } } });

  if (response.status !== 200 || data === undefined) {
    throw new Error("Failed to fetch note");
  }

  return data;
}

export async function updateNote(noteId: number, payload: NotePatchRequest): Promise<void> {
  const { response } = await PATCH("/notes/{note_id}", { body: payload, params: { path: { note_id: noteId } } });
  if (response.status !== 200) {
    throw new Error("Failed to update note");
  }
}

export async function deleteNote(noteId: number): Promise<void> {
  const { response } = await DELETE("/notes/{note_id}", { params: { path: { note_id: noteId } } });
  if (response.status !== 200) {
    throw new Error("Failed to delete note");
  }
}

export async function createNote(payload: NoteCreateRequest): Promise<number> {
  const { data, response } = await POST("/notes", { body: payload });
  if (response.status !== 201 || data === undefined || data.note_id === undefined) {
    throw new Error("Failed to create note");
  }
  return data.note_id;
}
