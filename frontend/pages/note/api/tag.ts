import { GET, POST, DELETE } from "shared/api";

export type Tag = {
  tag_id: number;
  name: string;
};

export async function getAllTags(): Promise<Tag[]> {
  const { data, response } = await GET("/tags/all");
  if (response.status !== 200 || !data) {
    throw new Error("Error fetching tags");
  }
  return data;
}

export async function getTagsForNote(noteId: number): Promise<Tag[]> {
  const { data, response } = await GET('/tags/{note_id}', { params: { path: { note_id: noteId}}});
  if (response.status !== 200 || !data) {
    throw new Error("Error fetching note tags");
  }
  return data;
}

export async function createTag(name: string): Promise<Tag> {
  const { data, response } = await POST("/tags/create", { body: { name } });
  if (response.status !== 201 || !data) {
    throw new Error("Error creating tag");
  }
  const tag: Tag = {
    tag_id: data.tag_id,
    name: name
  } 
  return tag;
}

export async function addTagToNote(noteId: number, tagId: number): Promise<void> {
  const { response } = await POST('/tags/{note_id}/{tag_id}', { params: { path: {tag_id: tagId, note_id: noteId }}});
  if (response.status !== 200) {
    throw new Error("Error adding tag to note");
  }
}

export async function removeTagFromNote(noteId: number, tagId: number): Promise<void> {
  const { response } = await DELETE('/tags/{note_id}/{tag_id}', { params: { path: { tag_id: tagId, note_id: noteId }}});
  if (response.status !== 200) {
    throw new Error("Error removing tag from note");
  }
}
