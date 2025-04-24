import { http, HttpResponse } from 'msw'
import type { LoginRequest, LoginResponse, NoteResponse, Notes } from 'shared/api'
import { type NoteCreateResponse, type NoteCreateRequest, type NotePatchRequest, type RegisterRequest, type ChatAnswerRequest, type TagCreateRequest } from 'shared/api'
import { type NotesRequest } from 'shared/api/models'

let notes: NoteResponse[] = [{
  note_id: 1,
  title: "note1",
  body: "abcd",
  created_at: "2025-03-14T10:28:47Z",
  updated_at: "2025-03-14T10:28:47Z",
},
{
  note_id: 2,
  title: "note2",
  body: "abcdd",
  created_at: "2025-03-13T10:28:47Z",
  updated_at: "2025-03-13T10:28:47Z",
},
{
  note_id: 3,
  title: "note3",
  body: "abcddd",
  created_at: "2025-03-12T10:28:47Z",
  updated_at: "2025-03-12T10:28:47Z",
}]

for (let i = 4; i < 100; i++) {
  notes.push({
    note_id: i,
    title: "note" + i.toString(),
    body: "note" + i.toString(),
    created_at: "2025-03-12T10:28:47Z",
    updated_at: "2025-03-12T10:28:47Z"
  })
}

let noteTitle = "note1"
let noteBody = "body"

let noteTags = [
  { tag_id: 1, name: "work" },
  { tag_id: 2, name: "personal" },
]

let allTags = [
  { tag_id: 1, name: "work" },
  { tag_id: 2, name: "personal" },
  { tag_id: 3, name: "urgent" },
  { tag_id: 4, name: "test" }
]

const chatHistory: Array<{
  query_id: number;
  query: string;
  response: string;
  created_at: string;
}> = [
    {
      query_id: 1,
      query: "How can I improve my productivity?",
      response: "Try using the Pomodoro technique.",
      created_at: "2025-03-14T10:00:00Z"
    },
    {
      query_id: 2,
      query: "What is the weather like?",
      response: "It's sunny today.",
      created_at: "2025-03-14T09:50:00Z"
    },
    {
      query_id: 3,
      query: "Tell me a joke.",
      response: "Why did the chicken cross the road? To get to the other side!",
      created_at: "2025-03-14T09:40:00Z"
    }
  ]

let checklistIdCounter = 1;
let itemIdCounter = 1;

interface MockChecklist {
  checklist_id: number;
  note_id: number;
  title: string;
  created_at: string;
  updated_at: string;
  items: Array<{
    item_id: number;
    text: string;
    completed: boolean;
    created_at: string;
    updated_at: string;
  }>;
}

const checklists: MockChecklist[] = [];

export const handlers = [
  http.post<never, LoginRequest>('/auth/login', async ({ request }) => {
    const creds = await request.json()

    const username = creds.username
    const password = creds.password

    if (username !== "john") {
      return HttpResponse.text('', { status: 404 })
    }
    if (password !== "123") {
      return HttpResponse.text('', { status: 401 })
    }

    return HttpResponse.json<LoginResponse>({
      access_token: 'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiIxMjM0NTY3ODkwIiwibmFtZSI6ImpvaG4iLCJpYXQiOjE1MTYyMzkwMjJ9.Vs_Aa0a3lNNSk1rGFzrejXqKk0dfqRQFZQWbhl2JHmU'
    })
  }),
  http.post<never, RegisterRequest>('/auth/register', async ({ request }) => {
    return HttpResponse.text('', { status: 201 })
  }),
  http.get('/notes', async ({ request, params }) => {
    const url = new URL(request.url)

    const page = parseInt(url.searchParams.get('page') ?? '1')
    const limit = parseInt(url.searchParams.get('limit') ?? '20')

    const offset = limit * (page - 1)

    let filteredNotes = notes
    const query = url.searchParams.get('query')
    if (query) {
      filteredNotes = notes.filter(n => n.title.includes(query))
    }

    return HttpResponse.json<Notes>({
      notes: filteredNotes.slice(offset, offset + limit),
      total_count: filteredNotes.length
    },)
  }),
  http.get('/notes/1', async ({ request }) => {
    return HttpResponse.json<NotePatchRequest>({
      title: noteTitle,
      body: noteBody,
      created_at: "2025-03-14T10:28:47Z",
      updated_at: "2025-03-14T10:28:47Z",
    })
  }),
  http.patch<never, NotePatchRequest>('/notes/1', async ({ request }) => {
    const body = await request.json()
    if (!body.title || !body.body) {
      return HttpResponse.text('', { status: 400 })
    }

    noteTitle = body.title
    noteBody = body.body

    return HttpResponse.text('', { status: 200 })
  }),
  http.delete('/notes/1', async ({ request }) => {
    return HttpResponse.text('', { status: 200 })
  }),
  http.post<never, NoteCreateRequest>('/notes', async ({ request }) => {
    return HttpResponse.json<NoteCreateResponse>({ note_id: 1 }, { status: 201 })
  }),
  http.post<never, ChatAnswerRequest>('/ai/answer', async ({ request }) => {
    const { question } = await request.json()
    if (!question || question.length < 1 || question.length > 4096) {
      return HttpResponse.text('', { status: 400 })
    }
    const query_id = Math.floor(Math.random() * 1000) + 4
    const answer = `This is a mocked answer for: "${question}"`
    chatHistory.unshift({
      query_id,
      query: question,
      response: answer,
      created_at: new Date().toISOString()
    })
    return HttpResponse.json({ query_id, answer })
  }),
  http.get('/history', async ({ request }) => {
    const url = new URL(request.url)
    const page = Number(url.searchParams.get("page")) || 1
    const limit = Number(url.searchParams.get("limit")) || 20
    const start = (page - 1) * limit
    const paginated = chatHistory.slice(start, start + limit)
    return HttpResponse.json({ history: paginated })
  }),
  http.post<never, TagCreateRequest>('/tags/create', async ({ request }) => {
    const { name } = await request.json()
    if (!name || name.length < 1 || name.length > 50) {
      return HttpResponse.text('', { status: 400 })
    }
    if (allTags.find(tag => tag.name === name)) {
      return HttpResponse.text('', { status: 409 })
    }
    const newTagId = allTags.length + 1
    const newTag = { tag_id: newTagId, name }
    allTags.push(newTag)
    return HttpResponse.json({ tag_id: newTagId }, { status: 201 })
  }),
  http.get('/tags/all', async () => {
    return HttpResponse.json(allTags)
  }),
  http.get('/tags/1', async () => {
    return HttpResponse.json(noteTags)
  }),
  http.post('/tags/1/:tag_id', async ({ params }) => {
    const tagId = Number(params.tag_id)
    const tag = allTags.find(t => t.tag_id === tagId)
    if (!tag) {
      return HttpResponse.text('', { status: 404 })
    }
    if (noteTags.find(t => t.tag_id === tagId)) {
      return HttpResponse.text('', { status: 409 })
    }
    noteTags.push(tag)
    return HttpResponse.text('', { status: 200 })
  }),
  http.delete('/tags/1/:tag_id', async ({ params }) => {
    const tagId = Number(params.tag_id)
    const index = noteTags.findIndex(t => t.tag_id === tagId)
    if (index === -1) {
      return HttpResponse.text('', { status: 404 })
    }
    noteTags.splice(index, 1)
    return HttpResponse.text('', { status: 200 })
  }),
  http.get('/suggest/queries', async () => {
    return HttpResponse.json(['Remind me about a meeting...', 'Tell me a joke', 'What do I need to buy?'])
  }),
  http.post('/ai/generate_tags/:note_id', async () => {
    return HttpResponse.json(["ideas", "things"], { status: 200 })
  }),
  http.post('/suggest/tags/:note_id', async () => {
    return HttpResponse.json(["urgent", "test"], { status: 200 })
  }),
  http.post<{ note_id: string }, { title: string }>('/checklist/:note_id', async ({ request, params }) => {
    const noteId = Number(params.note_id);
    const { title } = await request.json();

    if (!title || title.length < 1 || title.length > 256) {
      return HttpResponse.json({ message: 'Invalid title length' }, { status: 400 });
    }

    const now = new Date().toISOString();
    const newChecklist: MockChecklist = {
      checklist_id: checklistIdCounter++,
      note_id: noteId,
      title,
      created_at: now,
      updated_at: now,
      items: [],
    };
    checklists.push(newChecklist);
    return HttpResponse.json({ checklist_id: newChecklist.checklist_id }, { status: 200 });
  }),
  http.get<{ note_id: string }, { title: string }>('/checklist/of_note/:note_id', async ({ request, params }) => {
    return HttpResponse.json(checklists.map(c => ({ checklist_id: c.checklist_id })), { status: 200 });
  }),
  http.get<{ checklist_id: string }>('/checklist/:checklist_id', async ({ params }) => {
    const id = Number(params.checklist_id);
    const c = checklists.find(x => x.checklist_id === id);
    if (!c) return HttpResponse.text('', { status: 404 });
    const { title, created_at, updated_at, items } = c;
    return HttpResponse.json({ title, created_at, updated_at, items }, { status: 200 });
  }),

  http.patch<{ checklist_id: string }, { title: string }>('/checklist/:checklist_id', async ({ request, params }) => {
    const id = Number(params.checklist_id);
    const { title } = await request.json();
    if (!title || title.length < 1 || title.length > 256) {
      return HttpResponse.json({ message: 'Invalid title length' }, { status: 400 });
    }
    const c = checklists.find(x => x.checklist_id === id);
    if (!c) return HttpResponse.text('', { status: 404 });
    c.title = title;
    c.updated_at = new Date().toISOString();
    return HttpResponse.text('', { status: 200 });
  }),

  http.delete<{ checklist_id: string }>('/checklist/:checklist_id', async ({ params }) => {
    const id = Number(params.checklist_id);
    const idx = checklists.findIndex(x => x.checklist_id === id);
    if (idx === -1) return HttpResponse.text('', { status: 404 });
    checklists.splice(idx, 1);
    return HttpResponse.text('', { status: 200 });
  }),

  http.post<{ checklist_id: string }, { text: string }>('/checklist/:checklist_id/item', async ({ request, params }) => {
    const checklistId = Number(params.checklist_id);
    const { text } = await request.json();
    if (!text || text.length < 1 || text.length > 512) {
      return HttpResponse.json({ message: 'Invalid text length' }, { status: 400 });
    }
    const c = checklists.find(x => x.checklist_id === checklistId);
    if (!c) return HttpResponse.text('', { status: 404 });

    const now = new Date().toISOString();
    const newItem = {
      item_id: itemIdCounter++,
      text,
      completed: false,
      created_at: now,
      updated_at: now,
    };
    c.items.push(newItem);
    c.updated_at = now;
    return HttpResponse.json({ item_id: newItem.item_id }, { status: 200 });
  }),

  http.patch<{ item_id: string }, { text?: string; status?: boolean }>(
    '/checklist/item/:item_id',
    async ({ request, params }) => {
      const itemId = Number(params.item_id);
      const patch = await request.json();
      const allItems = checklists.flatMap(c => c.items);
      const it = allItems.find(i => i.item_id === itemId);
      if (!it) return HttpResponse.text('', { status: 404 });

      if (patch.text !== undefined) {
        if (patch.text.length < 1 || patch.text.length > 512) {
          return HttpResponse.json({ message: 'Invalid text length' }, { status: 400 });
        }
        it.text = patch.text;
      }
      if (patch.status !== undefined) {
        it.completed = patch.status;
      }
      it.updated_at = new Date().toISOString();
      const parent = checklists.find(c => c.items.some(x => x.item_id === itemId))!;
      parent.updated_at = it.updated_at;
      return HttpResponse.text('', { status: 200 });
    }
  ),

  http.delete<{ item_id: string }>('/checklist/item/:item_id', async ({ params }) => {
    const itemId = Number(params.item_id);
    const parent = checklists.find(c => c.items.some(i => i.item_id === itemId));
    if (!parent) return HttpResponse.text('', { status: 404 });
    parent.items = parent.items.filter(i => i.item_id !== itemId);
    parent.updated_at = new Date().toISOString();
    return HttpResponse.text('', { status: 200 });
  }),
]
