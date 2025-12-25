import React, { InputHTMLAttributes } from 'react';

interface InputProps extends InputHTMLAttributes<HTMLInputElement> {
  label?: string;
}

const styles = {
  input: {
    width: '100%',
    padding: '16px',
    background: 'var(--input-bg)',
    border: '1px solid var(--input-border)',
    borderRadius: '14px',
    fontSize: '16px',
    color: 'var(--input-text)',
    outline: 'none',
    transition: 'border-color 0.3s ease, transform 0.1s ease'
  } as React.CSSProperties,
  label: {
    display: 'block',
    fontSize: '12px',
    fontWeight: 600,
    color: 'var(--text-secondary)',
    marginBottom: '8px',
    textTransform: 'uppercase',
    letterSpacing: '0.05em'
  } as React.CSSProperties,
  wrapper: { marginBottom: '24px' }
};

const Input: React.FC<InputProps> = ({ label, style, ...props }) => {
  return (
    <div style={styles.wrapper}>
      {label && <label style={styles.label}>{label}</label>}
      <input 
        style={{ ...styles.input, ...style }} 
        {...props} 
      />
    </div>
  );
};

export default Input;