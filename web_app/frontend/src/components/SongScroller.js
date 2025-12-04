import './SongScroller.css';

export default function SongScroller() {
    const songList = Array.from({ length: 21 }, (_, i) => ({
        id: i,
        songName: `Song ${i}`,
        artistName: `Artist ${i}`,
    }));

    return (
        <div className="SongScroller">
            {songList.map(song => (
                <button
                    className="songButton"
                    key={song.id}
                    onClick={() => alert(`Clicked ${song.songName}`)}
                >
                    <span style={{ fontSize: '1rem' }}>{song.songName}</span>
                    <br />
                    <span style={{ fontSize: '0.83rem' }}>{song.artistName}</span>
                </button>
            ))}
        </div>
    );
}
