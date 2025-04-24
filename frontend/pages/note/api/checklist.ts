import { POST, GET, PATCH, DELETE } from "shared/api";

export interface ChecklistItem {
  item_id: number;
  text: string;
  completed: boolean;
  created_at: string;
  updated_at: string;
}

export interface Checklist {
  checklist_id: number;
  title: string;
  created_at: string;
  updated_at: string;
  items: ChecklistItem[];
}

export async function getChecklistsForNote(
  checklists_id: number[]
): Promise<Checklist[]> {
  let checklists: Checklist[] = Array.of()

  for (var id of checklists_id) {
      checklists.push(await getChecklistById(id))
  }

  return checklists
}

export async function getChecklistById(
  checklistId: number
): Promise<Checklist> {
  const { data, response } = await GET("/checklist/{checklist_id}", {
    params: { path: { checklist_id: checklistId } },
  });
  if (response.status !== 200 || !data) {
    throw new Error("Failed to fetch checklist");
  }
  return { checklist_id: checklistId, ...data };
}

export async function createChecklist(
  noteId: number,
  title: string
): Promise<number> {
  const { data, response } = await POST("/checklist/{note_id}", {
    params: { path: { note_id: noteId } },
    body: { title },
  });
  if (response.status !== 200 || !data?.checklist_id) {
    throw new Error("Failed to create checklist");
  }
  return data.checklist_id;
}

export async function updateChecklistTitle(
  checklistId: number,
  title: string
): Promise<void> {
  const { response } = await PATCH("/checklist/{checklist_id}", {
    params: { path: { checklist_id: checklistId } },
    body: { title },
  });
  if (response.status !== 200) {
    throw new Error("Failed to update checklist title");
  }
}

export async function deleteChecklist(
  checklistId: number
): Promise<void> {
  const { response } = await DELETE("/checklist/{checklist_id}", {
    params: { path: { checklist_id: checklistId } },
  });
  if (response.status !== 200) {
    throw new Error("Failed to delete checklist");
  }
}

export async function addChecklistItem(
  checklistId: number,
  text: string
): Promise<number> {
  const { data, response } = await POST("/checklist/{checklist_id}/item", {
    params: { path: { checklist_id: checklistId } },
    body: { text },
  });
  if (response.status !== 200 || !data?.item_id) {
    throw new Error("Failed to add item");
  }
  return data.item_id;
}

export async function updateChecklistItem(
  itemId: number,
  patch: { text?: string; status?: boolean }
): Promise<void> {
  const { response } = await PATCH("/checklist/item/{item_id}", {
    params: { path: { item_id: itemId } },
    body: patch,
  });
  if (response.status !== 200) {
    throw new Error("Failed to update item");
  }
}

export async function deleteChecklistItem(
  itemId: number
): Promise<void> {
  const { response } = await DELETE("/checklist/item/{item_id}", {
    params: { path: { item_id: itemId } },
  });
  if (response.status !== 200) {
    throw new Error("Failed to delete item");
  }
}
