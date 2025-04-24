import { GET, DELETE } from "shared/api";
import { backendBaseUrl } from "shared/config";

export interface AttachmentReference {
  attachment_id: number;
}

export interface AttachmentData {
  file_name: string;
  content: string;
}

export async function addAttachmentToNote(
  noteId: number,
  file: File
): Promise<number> {
  const form = new FormData();
  form.append("file", file);

  let response = await fetch(backendBaseUrl + "/attachment/" + noteId.toString(), {
    method: "POST",
    body: form
  })
  const data = await response.json()
  if (response.status !== 200 || !data?.attachment_id) {
    throw new Error("Failed to add attachment");
  }
  return data.attachment_id;
}

export async function getAttachmentsForNote(
  noteId: number
): Promise<AttachmentReference[]> {
  const { data, response } = await GET("/attachment/of_note/{note_id}", {
    params: { path: { note_id: noteId } },
  });
  if (response.status !== 200 || !data) {
    throw new Error("Failed to fetch attachments");
  }
  return data;
}

export async function getAttachmentFromNote(
  attachmentId: number
): Promise<AttachmentData> {
  const { data, response } = await GET("/attachment/{attachment_id}", {
    params: { path: { attachment_id: attachmentId } },
  });
  if (response.status !== 200 || !data) {
    throw new Error("Failed to fetch attachment");
  }
  return data;
}

export async function deleteAttachmentFromNote(
  attachmentId: number
): Promise<void> {
  const { response } = await DELETE("/attachment/{attachment_id}", {
    params: { path: { attachment_id: attachmentId } },
  });
  if (response.status !== 200) {
    throw new Error("Failed to delete attachment");
  }
}
