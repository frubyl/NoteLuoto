import type { operations, components } from "./v1";
import { z } from 'zod'

export const LoginRequestSchema = z
  .object({
    username: z.string().nonempty({ message: "Username is required" }),
    password: z.string().nonempty({ message: "Password is required" })
  });

export type LoginRequest = z.infer<typeof LoginRequestSchema>
export type LoginResponse = operations['loginUser']['responses']['200']['content']['application/json']
export type NoteResponse = components['schemas']['NoteResponse']
export type NotesRequest = operations['getNotesWithSearchAndPagination']['parameters']['query']