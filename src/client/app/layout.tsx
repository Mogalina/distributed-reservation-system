import './globals.css';
import { UserProvider } from '../context/UserContext';
import { Metadata } from 'next';

export const metadata: Metadata = {
  title: 'Reservation System',
  description: 'Ticket reservation and payment client',
};

export default function RootLayout({
  children,
}: {
  children: React.ReactNode;
}) {
  return (
    <html lang="en">
      <body>
        <UserProvider>
          {children}
        </UserProvider>
      </body>
    </html>
  );
}
