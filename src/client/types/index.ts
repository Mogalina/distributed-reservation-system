export interface User {
  userId: string;
  username: string;
  token: string;
}

export interface AuthResponse {
  userId: string;
  token: string;
  error?: string;
  message?: string;
}

export interface AuthRequestData {
  username: string;
  password: string;
  nationalId?: string;
}