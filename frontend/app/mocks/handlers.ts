import { http, HttpResponse } from 'msw'
import type { LoginRequest, LoginResponse, NoteResponse } from 'shared/api'
import { type NoteCreateResponse, type NoteCreateRequest, type NotePatchRequest, type RegisterRequest } from 'shared/api/models'

let noteTitle = "note1"
let noteBody = "body"

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
  })
]