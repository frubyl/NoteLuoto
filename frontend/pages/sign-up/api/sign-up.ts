import { POST } from "shared/api";
import type { RegisterRequest } from "shared/api/models";

export type SignUpResult = {
  status: number
}

export async function signUp(body: RegisterRequest): Promise<SignUpResult> {
  const { response } = await POST("/auth/register", { body: body });

  return {
    status: response.status
  };
}

