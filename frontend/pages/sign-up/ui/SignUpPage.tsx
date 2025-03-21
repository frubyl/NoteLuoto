import { useForm } from "react-hook-form"
import type { SubmitHandler } from "react-hook-form";
import { zodResolver } from "@hookform/resolvers/zod";
import type { LoginRequest } from "shared/api";
import { signUp } from "../api/sign-up";
import { useNavigate } from "react-router";
import { RegisterRequestSchema, type RegisterRequest } from "shared/api/models";

export function SignUpPage() {
  let navigate = useNavigate()

  const {
    register,
    handleSubmit,
    setError,
    formState: { errors }
  } = useForm<RegisterRequest>({
    resolver: zodResolver(RegisterRequestSchema)
  })

  const onSubmit: SubmitHandler<LoginRequest> = async function (body) {
    const { status } = await signUp(body)

    if (status === 400) {
      setError('username', {
        message: 'Incorrect sign up details'
      })
      return
    }

    if (status === 409) {
      setError('username', {
        message: 'A user with this username already exists'
      })
      return
    }

    if (status !== 201) {
      setError('username', {
        message: 'Unexpected error'
      })
      return
    }

    navigate("/login")
  }

  return (
    <>
      <div className="flex min-h-full flex-col items-center justify-center px-6 py-12 lg:px-8">
        <div className="w-full max-w-md pb-8 rounded-lg shadow-md bg-[#3a3844]">
          <div className="sm:mx-auto sm:w-full sm:max-w-sm">
            <h2 className="mt-10 text-center text-2xl/9 font-bold tracking-tight text-white">Sign up</h2>
          </div>

          <div className="mt-10 sm:mx-auto sm:w-full sm:max-w-sm">
            <form className="space-y-6" onSubmit={handleSubmit(onSubmit)}>
              <div>
                <label className="block text-sm/6 font-medium text-white">Username</label>
                <div className="mt-2">
                  <input className="block w-full rounded-md bg-white px-3 py-1.5 text-base text-gray-900 outline-1 -outline-offset-1 outline-gray-300 placeholder:text-gray-400 focus:outline-2 focus:-outline-offset-2 focus:outline-indigo-600 sm:text-sm/6" required {...register("username")} />
                  {errors.username && <label className="block text-sm/6 font-medium text-red-500">{errors.username.message}</label>}
                </div>
              </div>

              <div>
                <div className="flex items-center justify-between">
                  <label htmlFor="password" className="block text-sm/6 font-medium text-white">Password</label>
                </div>
                <div className="mt-2">
                  <input className="block w-full rounded-md bg-white px-3 py-1.5 text-base text-gray-900 outline-1 -outline-offset-1 outline-gray-300 placeholder:text-gray-400 focus:outline-2 focus:-outline-offset-2 focus:outline-indigo-600 sm:text-sm/6" required {...register("password")} />
                  {errors.password && <label className="block text-sm/6 font-medium text-red-500">{errors.password.message}</label>}
                </div>
              </div>

              <div className="flex gap-x-4">
                <button type="submit" className="flex w-full justify-center rounded-md text-center block bg-[#2b2930] text-[#d0bcfe] pt-2 pb-2 text-sm rounded-lg shadow hover:shadow-md hover:bg-[#38343d] transition">Sign up</button>
              </div>
            </form>
          </div>
        </div>
      </div>
    </>
  );
}