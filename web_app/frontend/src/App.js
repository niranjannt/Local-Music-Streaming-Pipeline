import './App.css';
import { useState } from 'react';

import MusicPlayer from './components/MusicPlayer';
import SongScroller from './components/SongScroller';

function App() {
    const [isPlaying, setIsPlaying] = useState(false);

    const togglePlay = () => {
        setIsPlaying(!isPlaying);
    };
    return (
        <div className= "App">
            <MusicPlayer isPlaying={isPlaying} togglePlay={togglePlay} />
            <SongScroller/>
        </div>
    );
}

export default App;