import '@testing-library/jest-dom'
import { render, screen, waitFor } from '@testing-library/react';
import userEvent from '@testing-library/user-event';
import { SignInPage } from './SignInPage';
import { signIn } from '../api/sign-in';
import { setAuthToken } from 'shared/auth';
import { BrowserRouter, Routes, Route } from 'react-router';

jest.mock('../api/sign-in');
jest.mock('shared/auth');
const mockSignIn = signIn as jest.MockedFunction<typeof signIn>;
const mockSetAuthToken = setAuthToken as jest.MockedFunction<typeof setAuthToken>;

const mockNavigate = jest.fn();
const mockUseSearchParams = jest.fn().mockReturnValue([new URLSearchParams(), jest.fn()]);
jest.mock('react-router', () => ({
  ...(jest.requireActual('react-router') as any),
  useNavigate: () => mockNavigate,
  useSearchParams: () => mockUseSearchParams(),
}));

function renderWithRouter() {
  return render(
    <BrowserRouter>
      <Routes>
        <Route path="*" element={<SignInPage />} />
      </Routes>
    </BrowserRouter>
  );
}

describe('SignInPage', () => {
  beforeEach(() => {
    jest.clearAllMocks();
  });

  it('shows username error on 404', async () => {
    mockSignIn.mockResolvedValue({ status: 404, access_token: undefined });
    renderWithRouter();

    await userEvent.type(screen.getByLabelText(/Username/i), 'alice');
    await userEvent.type(screen.getByLabelText(/Password/i), 'secret');
    userEvent.click(screen.getByRole('button', { name: /Sign in/i }));

    expect(await screen.findByText('Username not found')).toBeInTheDocument();
  });

  it('shows password error on 401', async () => {
    mockSignIn.mockResolvedValue({ status: 401, access_token: undefined });
    renderWithRouter();

    await userEvent.type(screen.getByLabelText(/Username/i), 'alice');
    await userEvent.type(screen.getByLabelText(/Password/i), 'wrong');
    await userEvent.click(screen.getByRole('button', { name: /Sign in/i }));

    expect(await screen.findByText('Incorrect password')).toBeInTheDocument();
  });

  it('sets token and navigates to / on success', async () => {
    mockSignIn.mockResolvedValue({ status: 200, access_token: 'tok_123' });
    mockUseSearchParams.mockReturnValue([new URLSearchParams(), jest.fn()]);

    renderWithRouter();

    await userEvent.type(screen.getByLabelText(/Username/i), 'alice');
    await userEvent.type(screen.getByLabelText(/Password/i), 'secret');
    await userEvent.click(screen.getByRole('button', { name: /Sign in/i }));

    expect(mockSetAuthToken).toHaveBeenCalledWith('tok_123');
    expect(mockNavigate).toHaveBeenCalledWith('/');
  });

  it('navigates to returnTo when present', async () => {
    mockSignIn.mockResolvedValue({ status: 200, access_token: 'tok_456' });
    const params = new URLSearchParams({ returnTo: '/dashboard' });
    mockUseSearchParams.mockReturnValue([params, jest.fn()]);

    renderWithRouter();

    await userEvent.type(screen.getByLabelText(/Username/i), 'bob');
    await userEvent.type(screen.getByLabelText(/Password/i), 'hunter2');
    await userEvent.click(screen.getByRole('button', { name: /Sign in/i }));

    await waitFor(() => {
      expect(mockSetAuthToken).toHaveBeenCalledWith('tok_456');
      expect(mockNavigate).toHaveBeenCalledWith('/dashboard');
    });
  });
});
