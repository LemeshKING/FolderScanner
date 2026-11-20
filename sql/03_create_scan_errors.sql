-- @statement

CREATE TABLE IF NOT EXISTS scan_errors
(
	error_id INTEGER PRIMARY KEY AUTOINCREMENT,
	
	scan_id INTEGER NOT NULL,
	
	path TEXT NOT NULL,
	
	message TEXT NOT NULL,
	
	FOREIGN KEY (scan_id)
		REFERENCES scans(scan_id)
		ON DELETE CASCADE
);

-- @statement

CREATE INDEX IF NOT EXISTS idx_errors_scan_id
	ON scan_errors(scan_id);