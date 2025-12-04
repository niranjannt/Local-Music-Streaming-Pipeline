import './App.css';
import {useEffect, useState} from 'react';

import undefinedCover from "./assets/undefined.jpg";
import need2Cover from "./assets/need2.jpg";
import six16inLACover from "./assets/616inLA.jpg";
import backIcon from "./assets/back.png";
import pauseIcon from "./assets/pause.png";
import playIcon from "./assets/play.png";
import forwardIcon from "./assets/forward.png";

function App() {
    const songList = [
        {id: 0, songName: "undefined", artistName: "Dazegxd", cover: undefinedCover},
        {id: 1, songName: "616 in LA", artistName: "Kendrick Lamar", cover: six16inLACover},
        {id: 2, songName: "Need 2", artistName: "Pinegrove", cover: need2Cover},
    ];
    const [activeSong, setActiveSong] = useState(null);
    const [isPlaying, setIsPlaying] = useState(false);

    return (
        <div className= "App">
            <MusicPlayer
                isPlaying = {isPlaying}
                setIsPlaying = {setIsPlaying}
                activeSong = {activeSong}
            />
            <SongScroller
                activeSong = {activeSong}
                setActiveSong = {setActiveSong}
                songList = {songList}
                setIsPlaying={setIsPlaying}
            />
        </div>
    );
}

function MusicPlayer({isPlaying, setIsPlaying, activeSong}) {
    const handlePlayButton = async event => {
        const newPlayingState = !isPlaying
        setIsPlaying(newPlayingState);

        const data = {id : activeSong.id};

        let apiURL;
        if (!isPlaying) {
            apiURL = "http://localhost:8080/api/audio/play"
        }
        else {
            apiURL = "http://localhost:8080/api/audio/pause"
        }


        try {
            const response = await fetch(apiURL, { method: "POST" });
            if (!response.ok) {
                alert("Error: " + response.status);
                setIsPlaying(!newPlayingState);
            }
        }
        catch (error) {
            console.error("Error:", error);
            setIsPlaying(!newPlayingState);
        }
    };
    return (
        <div className="musicPlayer">
            <div className="imageDiv">
                <img className="coverImage" src={activeSong ? activeSong.cover : undefinedCover} alt="cover" />
            </div>

            <div className="controls">
                <button className="iconButton">
                    <img src={backIcon} alt="back" />
                </button>

                <button className="iconButton" onClick={handlePlayButton}>
                    <img src={isPlaying ? pauseIcon : playIcon} alt="play/pause" />
                </button>

                <button className="iconButton">
                    <img src={forwardIcon} alt="forward" />
                </button>
            </div>
        </div>
    );
}

function SongScroller({activeSong, setActiveSong, songList, setIsPlaying}) {
    const handleSongChange = async (song) => {
        setActiveSong(song);
        setIsPlaying(true);

        try {
            const response = await fetch(
                `http://localhost:8080/api/audio/load/${song.id}`,
                {method: "POST"}
            );

            if (!response.ok) {
                alert("Error loading song");
                setIsPlaying(false);
            }
        }
        catch (error) {
            console.error("Error:", error);
            alert("Failed to load song");
            setIsPlaying(false);
        }
    };

    return (
        <div className="SongScroller">
            {songList.map(song => (
                <button
                    key={song.id}
                    className={`songButton ${activeSong?.id === song.id ? 'activeSong' : ''}`}
                    onClick={() => handleSongChange(song)}
                >
                    <span style={{fontSize: '1rem'}}>{song.songName}</span>
                    <br/>
                    <span style={{fontSize: '0.83rem'}}>{song.artistName}</span>
                </button>
            ))}
        </div>
    );
}

export default App;