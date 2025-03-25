import { useEffect, useState } from 'react';
import {  
  addTagToNote,  
  getAllTags, 
  createTag, 
  getTagsForNote,
  type Tag 
} from "../api/tag";

export function TagModal({
  noteId,
  onClose,
  onTagAdded,
}: {
  noteId: number;
  onClose: () => void;
  onTagAdded: () => void;
}) {
  const [allTags, setAllTags] = useState<Tag[]>([]);
  const [searchTerm, setSearchTerm] = useState("");
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState("");

  useEffect(() => {
    (async () => {
      try {
        setLoading(true);
        const tags = await getAllTags();
        const noteTags = await getTagsForNote(noteId);
        const ids = noteTags.map(t => t.tag_id)
        setAllTags(tags.filter(t => !ids.includes(t.tag_id)));
      } catch (err) {
        setError("Error fetching tags.");
      } finally {
        setLoading(false);
      }
    })();
  }, []);

  const filteredTags = allTags.filter(tag =>
    tag.name.toLowerCase().includes(searchTerm.toLowerCase())
  );

  const handleAddTag = async (tagId: number) => {
    try {
      await addTagToNote(noteId, tagId);
      onTagAdded();
      setAllTags((tags) => tags.filter(t => t.tag_id != tagId))
    } catch (err) {
      setError("Error adding tag.");
    }
  };

  const handleCreateTag = async () => {
    if (!searchTerm.trim()) return;
    try {
      const newTag = await createTag(searchTerm.trim());
      await addTagToNote(noteId, newTag.tag_id);
      onTagAdded();
      setSearchTerm("")
    } catch (err) {
      setError("Error creating tag.");
    }
  };

  return (
    <div className="fixed inset-0 flex items-center justify-center bg-black bg-opacity-50 z-50">
      <div className="bg-white p-4 rounded w-96">
        <div className="flex justify-between items-center mb-4">
          <h2 className="text-lg font-bold">Select Tag</h2>
          <button onClick={onClose} className="text-red-500 font-bold">X</button>
        </div>
        <input
          type="text"
          placeholder="Search or create tag..."
          value={searchTerm}
          onChange={(e) => setSearchTerm(e.target.value)}
          className="border p-2 rounded w-full mb-4"
        />
        {loading && <p>Loading tags...</p>}
        {error && <p className="text-red-500">{error}</p>}
        <ul className="max-h-48 overflow-y-auto mb-4">
          {filteredTags.map(tag => (
            <li key={tag.tag_id} className="flex justify-between items-center p-2 border-b">
              <span>{tag.name}</span>
              <button
                onClick={() => handleAddTag(tag.tag_id)}
                className="bg-blue-500 text-white px-2 py-1 rounded"
              >
                Add
              </button>
            </li>
          ))}
          {searchTerm && !filteredTags.some(tag => tag.name.toLowerCase() === searchTerm.toLowerCase()) && (
            <li className="p-2">
              <button
                onClick={handleCreateTag}
                className="bg-green-500 text-white px-2 py-1 rounded w-full"
              >
                Create tag "{searchTerm}"
              </button>
            </li>
          )}
        </ul>
      </div>
    </div>
  );
}