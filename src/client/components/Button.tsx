import React, { ButtonHTMLAttributes } from 'react';

interface ButtonProps extends ButtonHTMLAttributes<HTMLButtonElement> {
  children: React.ReactNode;
}

const styles: React.CSSProperties = {
  width: '100%',
  padding: '18px',
  backgroundColor: 'var(--button-bg)',
  color: 'var(--button-text)',
  border: 'none',
  borderRadius: '14px',
  fontSize: '16px',
  fontWeight: 700,
  cursor: 'pointer',
  transition: 'transform 0.2s ease, opacity 0.2s ease',
  marginTop: '12px'
};

const Button: React.FC<ButtonProps> = ({ children, style, ...props }) => (
  <button 
    style={{ ...styles, ...style }} 
    {...props}
    onMouseDown={(e) => e.currentTarget.style.transform = 'scale(0.98)'}
    onMouseUp={(e) => e.currentTarget.style.transform = 'scale(1)'}
    onMouseLeave={(e) => e.currentTarget.style.transform = 'scale(1)'}
  >
    {children}
  </button>
);

export default Button;