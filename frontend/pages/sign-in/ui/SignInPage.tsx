import { useForm } from "react-hook-form"
import type { SubmitHandler } from "react-hook-form";
import { zodResolver } from "@hookform/resolvers/zod";
import { useState } from "react";
import { LoginRequestSchema } from "shared/api";
import type { LoginRequest } from "shared/api";
import { signIn } from "../api/sign-in";

export function SignInPage() {
  const {
    register,
    handleSubmit,
    setError,
    formState: { errors }
  } = useForm<LoginRequest>({
    resolver: zodResolver(LoginRequestSchema)
  })

  const [accessToken, setAccessToken] = useState<string | undefined>(undefined);

  const onSubmit: SubmitHandler<LoginRequest> = async function (body) {
    const { access_token, status } = await signIn(body)
    
    setAccessToken(undefined)

    if (status === 404) {
      setError('username', {
        message: 'Username not found'
      })
      return
    }

    if (status === 401) {
      setError('password', {
        message: 'Incorrect password'
      })
      return
    }

    if (status !== 200) {
      return
    }

    setAccessToken(access_token)
  }

  return (
    <>
      <div className="flex min-h-full flex-col justify-center px-6 py-12 lg:px-8">
        <div className="sm:mx-auto sm:w-full sm:max-w-sm">
          <h2 className="mt-10 text-center text-2xl/9 font-bold tracking-tight text-gray-900">Sign in to your account</h2>
          {accessToken && <label className="block break-words text-sm/6 font-medium text-green-900">Success! Your JWT: { accessToken }</label>}
        </div>

        <div className="mt-10 sm:mx-auto sm:w-full sm:max-w-sm">
          <form className="space-y-6" onSubmit={handleSubmit(onSubmit)}>
            <div>
              <label className="block text-sm/6 font-medium text-gray-900">Username</label>
              <div className="mt-2">
                <input className="block w-full rounded-md bg-white px-3 py-1.5 text-base text-gray-900 outline-1 -outline-offset-1 outline-gray-300 placeholder:text-gray-400 focus:outline-2 focus:-outline-offset-2 focus:outline-indigo-600 sm:text-sm/6" required {...register("username")} />
                {errors.username && <label className="block text-sm/6 font-medium text-red-900">{errors.username.message}</label>}
              </div>
            </div>

            <div>
              <div className="flex items-center justify-between">
                <label htmlFor="password" className="block text-sm/6 font-medium text-gray-900">Password</label>
              </div>
              <div className="mt-2">
                <input className="block w-full rounded-md bg-white px-3 py-1.5 text-base text-gray-900 outline-1 -outline-offset-1 outline-gray-300 placeholder:text-gray-400 focus:outline-2 focus:-outline-offset-2 focus:outline-indigo-600 sm:text-sm/6" required {...register("password")} />
                {errors.password && <label className="block text-sm/6 font-medium text-red-900">{errors.password.message}</label>}
              </div>
            </div>

            <div className="flex gap-x-4">
              <button type="submit" className="flex w-1/2 justify-center rounded-md bg-indigo-600 px-3 py-1.5 text-sm/6 font-semibold text-white shadow-xs hover:bg-indigo-500 focus-visible:outline-2 focus-visible:outline-offset-2 focus-visible:outline-indigo-600">Sign in</button>
              <button className="flex w-1/2 justify-center rounded-md bg-indigo-600 px-3 py-1.5 text-sm/6 font-semibold text-white shadow-xs hover:bg-indigo-500 focus-visible:outline-2 focus-visible:outline-offset-2 focus-visible:outline-indigo-600">Sign up</button>
            </div>
          </form>

        </div>
      </div>
    </>
  );
}