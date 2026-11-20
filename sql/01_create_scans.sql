-- @statement

CREATE TABLE IF NOT EXISTS scans
(
	scan_id INTEGER PRIMARY KEY AUTOINCREMENT,
	
	root_path TEXT not NULL,
	
	scanned_at_utc TEXT NOT NULL,
	
	status TEXT NOT NULL
		CHECK(status IN ('COMPLETE', 'PARTIAL', 'FAILED'))
);

-- @statement

CREATE INDEX IF NOT EXISTS idx_scans_root_path
	ON scans(root_path);
	
-- @statement	
	
CREATE INDEX IF NOT EXISTS idx_scans_date
	ON scans(scanned_at_utc);