import { GET } from "shared/api";
import type { NoteResponse } from "shared/api";

export type RecentNotes = {
  status: number,
  notes: NoteResponse[] | undefined
}

export async function getRecentNotes(num: number): Promise<RecentNotes> {
  const { data, response } = await GET("/notes", { params: { query: { page: 1, limit: num }}})
  
  return {
    status: response.status,
    notes: data
  }
}
