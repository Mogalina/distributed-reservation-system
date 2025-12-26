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

export interface Event {
  eventId: string;
  eventName: string;
  startTime: string;
  endTime: string;
  capacity: number;
  availableTickets?: number;
}

export interface PaginatedEvents {
  data: Event[];
  meta: {
    total: number;
    page: number;
    limit: number;
    totalPages: number;
  };
}
