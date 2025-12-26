import { AuthRequestData, AuthResponse, PaginatedEvents } from '../types';

const API_URL = 'http://127.0.0.1:8080';

const request = async <T>(endpoint: string, options: RequestInit = {}): Promise<T> => {
  try {
    const headers = {
      'Content-Type': 'application/json',
      ...(options.headers as Record<string, string>),
    };

    const res = await fetch(`${API_URL}/${endpoint}`, {
      ...options,
      headers,
    });

    const result = await res.json();
    if (!res.ok) {
      throw new Error(result.error || 'Request failed');
    }
    return result;

  } catch (error: any) {
    throw new Error(error.message || 'Network error');
  }
};

export const authRequest = (endpoint: string, data: AuthRequestData) => {
  return request<AuthResponse>(`auth/${endpoint}`, {
    method: 'POST',
    body: JSON.stringify(data),
  });
};

export const fetchEvents = (token: string, page: number, limit: number, search: string, type: string) => {
  const params = new URLSearchParams({
    page: page.toString(),
    limit: limit.toString(),
    search: search,
    type: type
  });

  return request<PaginatedEvents>(`events?${params.toString()}`, {
    method: 'GET',
    headers: {
      'Authorization': `Bearer ${token}`
    }
  });
};
