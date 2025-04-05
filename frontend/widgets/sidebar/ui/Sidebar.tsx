import { Link } from "react-router";

export function Sidebar() {
  return (
    <div className="text-black w-28 pl-1 pr-1 pt-6">
      <div className="fixed bottom-0 top-0 w-28 pl-1 pr-1 pt-6 bg-[#3a3844]">
        <nav className="flex flex-col space-y-3">
          <SidebarButton to="/" label="Home" />
          <SidebarButton to="/notes" label="All Notes" />
          <SidebarButton to="/search" label="Search" />
          <SidebarButton to="/chat" label="AI Chat" />
        </nav>
      </div>
    </div>
  );
}

function SidebarButton({ to, label }: { to: string, label: string }) {
  return (
    <Link to={to} className="text-center block bg-[#2b2930] text-[#d0bcfe] pt-2 pb-2 text-sm rounded-lg shadow hover:shadow-md hover:bg-[#38343d] transition">
      {label}
    </Link>
  );
}
