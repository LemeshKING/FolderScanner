-- @statement

CREATE TABLE IF NOT EXISTS scan_entries
(
	entry_id INTEGER PRIMARY KEY AUTOINCREMENT,
	
	scan_id INTEGER NOT NULL,
	
	parent_entry_id INTEGER NULL,
	
	name TEXT NOT NULL,
	
	relative_path TEXT NOT NULL,
	
	relative_path_key TEXT NOT NULL,
	
	entry_type TEXT NOT NULL
		CHECK (entry_type IN ('FILE', 'DIR')),
	
	size_bytes INTEGER NOT NULL DEFAULT 0
		CHECK (size_bytes >= 0),
	
	UNIQUE (scan_id, entry_id),
	
	UNIQUE (scan_id, relative_path_key),
	
	FOREIGN KEY (scan_id)
		REFERENCES scans(scan_id)
		ON DELETE CASCADE,
		
	FOREIGN KEY (scan_id, parent_entry_id)
		REFERENCES scan_entries(scan_id, entry_id)
		ON DELETE CASCADE,
		
	CHECK (
		entry_type = 'FILE'
		OR size_bytes = 0
	)
);

-- @statement

CREATE INDEX IF NOT EXISTS idx_entries_parent
	ON scan_entries(scan_id, parent_entry_id);
	
-- @statement	
	
CREATE INDEX IF NOT EXISTS idx_entries_type
	ON scan_entries(scan_id, entry_type);
	
-- @statement	
	
CREATE TRIGGER IF NOT EXISTS trg_parent_must_be_directory
BEFORE INSERT ON scan_entries
WHEN NEW.parent_entry_id IS NOT NULL
BEGIN
	SELECT RAISE(ABORT, 'Parent entry must be a directory')
	WHERE NOT EXISTS
	(
		SELECT 1
		FROM scan_entries
		WHERE entry_id = NEW.parent_entry_id
			AND scan_id = NEW.scan_id
			AND entry_type = 'DIR'
	);
END;