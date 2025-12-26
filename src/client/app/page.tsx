'use client';

import React, { useEffect, useState, useCallback } from 'react';
import { useRouter } from 'next/navigation';
import { useAuth } from '../hooks/useAuth';
import { fetchEvents } from '../services/api';
import { Event } from '../types';

const ITEMS_PER_PAGE = 5;

export default function HomePage() {
  const { user, logout, loading } = useAuth();
  const router = useRouter();

  const [searchTerm, setSearchTerm] = useState('');
  const [filterType, setFilterType] = useState<'ALL' | 'AVAILABLE' | 'EVENING'>('ALL');
  const [currentPage, setCurrentPage] = useState(1);
  
  const [events, setEvents] = useState<Event[]>([]);
  const [totalPages, setTotalPages] = useState(1);
  const [fetching, setFetching] = useState(false);

  useEffect(() => {
    if (!loading && !user) {
      router.push('/auth');
    }
  }, [user, loading, router]);

  const loadEvents = useCallback(async () => {
    if (!user || !user.token) {
      return; 
    }
    
    setFetching(true);
    try {
      const res = await fetchEvents(user.token, currentPage, ITEMS_PER_PAGE, searchTerm, filterType);
      setEvents(res.data);
      setTotalPages(res.meta.totalPages);

    } catch (err) {
      console.error("Failed to fetch events", err);
      if ((err as Error).message === 'Unauthorized') {
         logout();
      }

    } finally {
      setFetching(false);
    }
  }, [user, currentPage, searchTerm, filterType, logout]);

  useEffect(() => {
    const timer = setTimeout(() => {
      loadEvents();
    }, 300); 
    return () => clearTimeout(timer);
  }, [loadEvents]);

  if (loading || !user) {
    return null;
  }

  const handlePageChange = (newPage: number) => {
    if (newPage >= 1 && newPage <= totalPages) {
      setCurrentPage(newPage);
      window.scrollTo({ top: 0, behavior: 'smooth' });
    }
  };

  return (
    <div style={{ paddingBottom: '100px' }}>
      <nav className="glass-navbar">
        <div 
          onClick={() => window.location.reload()} 
          style={{ cursor: 'pointer', fontWeight: 700, fontSize: '18px', letterSpacing: '-0.02em' }}
        >
          EVENTS
        </div>
        <div style={{ display: 'flex', alignItems: 'center' }}>
          <span style={{ marginRight: '24px', fontWeight: 500, fontSize: '14px' }}>
            {user.username}
          </span>
          <button 
            onClick={() => { logout(); router.push('/auth'); }}
            style={{ 
              background: 'transparent', border: '1px solid #d2d2d7', 
              padding: '8px 16px', borderRadius: '20px', fontSize: '13px', 
              fontWeight: 600, cursor: 'pointer' 
            }}
          >
            Log Out
          </button>
        </div>
      </nav>

      <div className="dashboard-container">
        <h1 style={{ marginBottom: '20px', fontSize: '32px', fontWeight: 700 }}>Explore Events</h1>

        <input 
          type="text" 
          placeholder="Search events..." 
          className="search-bar"
          value={searchTerm}
          onChange={(e) => { 
            setSearchTerm(e.target.value); 
            setCurrentPage(1); 
          }}
        />
        
        <div className="filters">
          {['ALL', 'AVAILABLE', 'EVENING'].map((type) => (
            <button 
              key={type}
              className={`filter-chip ${filterType === type ? 'active' : ''}`}
              onClick={() => { 
                setFilterType(type as any); 
                setCurrentPage(1); 
              }}
            >
              {type === 'ALL' ? 'All Events' : type === 'AVAILABLE' ? 'Available Now' : 'Evening Only'}
            </button>
          ))}
        </div>

        <div>
          {fetching ? (
            <p style={{ textAlign: 'center', color: '#86868b', marginTop: '40px' }}>Loading events...</p>
          ) : events.length > 0 ? (
            events.map(evt => (
              <div key={evt.eventId} className="event-card">
                <div>
                  <h3 style={{ fontSize: '20px', fontWeight: 600, marginBottom: '4px' }}>{evt.eventName}</h3>
                  <span style={{ fontSize: '13px', color: '#a1a1a6', textTransform: 'uppercase', letterSpacing: '0.05em' }}>
                     Total Capacity: {evt.capacity}
                  </span>
                </div>
                <div className="event-info-right">
                  <div style={{ fontSize: '16px', fontWeight: 500 }}>
                    {evt.startTime} - {evt.endTime}
                  </div>
                  <div style={{ 
                    fontSize: '13px', 
                    marginTop: '4px', 
                    color: (evt.availableTickets || 0) > 0 ? '#4cd964' : '#ff3b30' 
                  }}>
                    {(evt.availableTickets || 0) > 0 
                      ? `${evt.availableTickets} tickets left` 
                      : 'Sold Out'}
                  </div>
                </div>
              </div>
            ))
          ) : (
            <p style={{ textAlign: 'center', color: '#86868b', marginTop: '40px' }}>
              No events match your criteria.
            </p>
          )}
        </div>

        {totalPages > 1 && (
          <div className="pagination">
            <button 
              disabled={currentPage === 1}
              onClick={() => handlePageChange(currentPage - 1)}
              style={{ background: 'none', border: 'none', cursor: currentPage === 1 ? 'not-allowed' : 'pointer', opacity: currentPage === 1 ? 0.3 : 1 }}
            >
              ← Previous
            </button>
            <span style={{ fontSize: '14px', color: '#86868b' }}>
              Page {currentPage} of {totalPages}
            </span>
            <button 
              disabled={currentPage === totalPages}
              onClick={() => handlePageChange(currentPage + 1)}
              style={{ background: 'none', border: 'none', cursor: currentPage === totalPages ? 'not-allowed' : 'pointer', opacity: currentPage === totalPages ? 0.3 : 1 }}
            >
              Next →
            </button>
          </div>
        )}
      </div>
    </div>
  );
}
