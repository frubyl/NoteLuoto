import { GET } from "shared/api";
import type { Notes } from "shared/api";

export async function getAllNotes(
  page: number,
  limit: number = 20,
  searchQuery?: string,
  searchType?: "semantic" | "exact"
): Promise<Notes> {
  const { data, response } = await GET("/notes", {
    params: {
      query: { page, limit, query: searchQuery, searchType }
    }
  });
  if (response.status !== 200 || !data) {
    throw new Error("Error fetching notes");
  }
  return data;
}
