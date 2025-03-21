import { useState, useEffect, useRef, type UIEvent } from "react";
import { getAIAnswer, getAIHistory } from "../api/chat";
import { Sidebar } from "widgets/sidebar";

export function ChatPage() {

  type Message = {
    type: "user" | "ai",
    content: string,
    created_at: string
  }

  const [chatHistory, setChatHistory] = useState<Message[]>([]);
  const [message, setMessage] = useState<string>("");
  const [loadingAnswer, setLoadingAnswer] = useState<boolean>(false);
  const [historyPage, setHistoryPage] = useState<number>(1);
  const [hasMoreHistory, setHasMoreHistory] = useState<boolean>(true);
  const [error, setError] = useState<string>("");
  const chatContainerRef = useRef<HTMLDivElement>(null);

  const loadHistory = async (page: number) => {
    try {
      const { history } = await getAIHistory(page);
      if (history.length < 20) {
        setHasMoreHistory(false);
      }
      const orderedHistory = [...history].reverse();
      const messages = orderedHistory.flatMap<Message>((record) => [
        {
          type: "user",
          content: record.query,
          created_at: record.created_at,
        },
        {
          type: "ai",
          content: record.response,
          created_at: record.created_at,
        },
      ]);
      setChatHistory((prev) => [...messages, ...prev]);
    } catch (err) {
      setError("Error loading chat history.");
    }
  };

  useEffect(() => {
    loadHistory(historyPage);
  }, [historyPage]);

  const handleScroll = (e: UIEvent<HTMLDivElement>) => {
    const container = e.currentTarget;
    if (container.scrollTop === 0 && hasMoreHistory) {
      setHistoryPage((prev) => prev + 1);
    }
  };

  const handleSend = async () => {
    if (!message.trim()) return;
    setError("");
    setLoadingAnswer(true);
    const now = new Date().toISOString();

    setChatHistory((prev) => [
      ...prev,
      { type: "user", content: message, created_at: now },
    ]);
    try {
      const aiResult = await getAIAnswer(message);
      const aiMessage: Message = {
        type: "ai",
        content: aiResult,
        created_at: new Date().toISOString(),
      };
      setChatHistory((prev) => [...prev, aiMessage]);
    } catch (err) {
      setError("Error getting AI answer.");
    } finally {
      setMessage("");
      setLoadingAnswer(false);
      if (chatContainerRef.current) {
        chatContainerRef.current.scrollTop = chatContainerRef.current.scrollHeight;
      }
    }
  };

  return (
    <div className="flex h-screen">
      <Sidebar />
      <div className="flex flex-col w-full p-4">
        <h1 className="text-white text-2xl font-bold mb-4">AI Chat</h1>

        <div
          ref={chatContainerRef}
          onScroll={handleScroll}
          className="flex-1 overflow-y-auto border border-gray-300 p-4 mb-4"
          style={{ scrollBehavior: "smooth" }}
        >
          {error && <div className="text-red-500">{error}</div>}
          {chatHistory.map((msg, idx) => (
            <div
              key={idx}
              className={`my-2 p-2 rounded ${
                msg.type === "user" ? "bg-blue-200 self-end" : "bg-gray-200 self-start"
              }`}
            >
              <p>{msg.content}</p>
              <span className="text-xs text-gray-500">
                {new Date(msg.created_at).toLocaleString()}
              </span>
            </div>
          ))}
        </div>

        <div className="flex">
          <input
            type="text"
            value={message}
            onChange={(e) => setMessage(e.target.value)}
            placeholder="Ask your question..."
            className="flex-1 p-2 border border-gray-300 rounded-l"
          />
          <button
            onClick={handleSend}
            disabled={loadingAnswer}
            className="p-2 bg-blue-500 text-white rounded-r hover:bg-blue-600 transition"
          >
            {loadingAnswer ? "Sending..." : "Send"}
          </button>
        </div>
      </div>
    </div>
  );
}
