import { http, HttpResponse } from 'msw'
import type { ChatHistoryResponse, LoginRequest, LoginResponse, NoteResponse } from 'shared/api'
import { type NoteCreateResponse, type NoteCreateRequest, type NotePatchRequest, type RegisterRequest, type ChatAnswerRequest } from 'shared/api'

let noteTitle = "note1"
let noteBody = "body"

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
  http.get('/notes', async ({ request }) => {
    return HttpResponse.json<NoteResponse[]>([{
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
    }])
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
  // New handler for AI answer
  http.post<never, ChatAnswerRequest>('/ai/answer', async ({ request }) => {
    const { question } = await request.json()
    // Basic validation
    if (!question || question.length < 1 || question.length > 4096) {
      return HttpResponse.text('', { status: 400 })
    }
    // Generate a simple mock answer and a random query ID
    const query_id = Math.floor(Math.random() * 1000) + 4 // starting from 4 since we already have 1-3
    const answer = `This is a mocked answer for: "${question}"`
    // Add to history (prepend to simulate most recent first)
    chatHistory.unshift({
      query_id,
      query: question,
      response: answer,
      created_at: new Date().toISOString()
    })
    return HttpResponse.json({ query_id, answer })
  }),
  // New handler for AI history with pagination
  http.get('/history', async ({ request }) => {
    const url = new URL(request.url)
    const page = Number(url.searchParams.get("page")) || 1
    const limit = Number(url.searchParams.get("limit")) || 20
    // Since our aiHistory array is sorted descending (most recent first),
    // we slice based on the requested page.
    const start = (page - 1) * limit
    const paginated = chatHistory.slice(start, start + limit)
    return HttpResponse.json({ history: paginated })
  })
]
