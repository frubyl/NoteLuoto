import { GET } from "shared/api";
import type { Notes } from "shared/api";

export async function getAllNotes(page: number, limit: number = 20): Promise<Notes> {
  const { data, response } = await GET("/notes", { params: { query: { page, limit } } });
  if (response.status !== 200 || !data) {
    throw new Error("Error fetching notes");
  }
  return data;
}
