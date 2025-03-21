import { POST, GET } from "shared/api";
import type { ChatAnswerRequest, ChatHistoryResponse } from "shared/api";

export async function getAIAnswer(question: string): Promise<string>  {
  const payload: ChatAnswerRequest = {
    question: question
  }
  const { data, response } = await POST("/ai/answer", { body: payload });
  if (response.status !== 200 || data?.answer === undefined) {
    throw new Error("Error getting AI answer");
  }
  return data.answer;
}

export async function getAIHistory(page: number, limit: number = 20): Promise<ChatHistoryResponse> {
  const { data, response } = await GET("/history", { params: { query: { page, limit } } });
  if (response.status !== 200 || data === undefined) {
    throw new Error("Error getting AI history");
  }
  return data;
}
