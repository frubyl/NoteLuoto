import type { operations, components } from "./v1";
import { z } from 'zod'

export const LoginRequestSchema = z
  .object({
    username: z.string().nonempty({ message: "Username is required" }),
    password: z.string().nonempty({ message: "Password is required" })
  });

export type LoginRequest = z.infer<typeof LoginRequestSchema>
export type LoginResponse = operations['loginUser']['responses']['200']['content']['application/json']

export const RegisterRequestSchema = z
  .object({
    username: z.string()
      .nonempty({ message: "Username is required" })
      .min(3, { message: "Username must be at least 3 characters long" })
      .max(30, { message: "Username must be at most 30 characters long" })
      .regex(new RegExp("[A-Za-z0-9_-]+"), { message: "Username can only contain: A-Z a-z 0-9 _ -"}),
    password: z.string()
      .nonempty({ message: "Password is required" })
      .min(8, { message: "Password must be at least 8 characters long" })
      .max(32, { message: "Password must be at most 32 characters long" })
      .regex(new RegExp(".*[a-z].*"), { message: "Password must contain at least one lowercase latin latter" })
      .regex(new RegExp(".*[0-9].*"), { message: "Password must contain at least one digit"})
  });
export type RegisterRequest = z.infer<typeof RegisterRequestSchema>

export type NotePatchRequest = components['schemas']['NotePatchRequest']
export type NoteCreateRequest = components['schemas']['NoteCreateRequest']
export type Notes = components['schemas']['Notes']
export type NoteResponse = components['schemas']['NoteResponse']
export type NoteGetResponse = components['schemas']['NoteGetResponse']
export type NotesRequest = operations['getNotesWithSearchAndPagination']['parameters']['query']
export type NoteCreateResponse = operations['createNote']['responses']['201']['content']['application/json']

export type ChatAnswerResponse = operations['getAIAnswer']['responses']['200']['content']['application/json']
export type ChatHistoryResponse = operations['getAIHistory']['responses']['200']['content']['application/json']
export type ChatAnswerRequest = operations['getAIAnswer']['requestBody']['content']['application/json']

export type TagCreateRequest = operations['createTag']['requestBody']['content']['application/json']
