import './MusicPlayer.css';
import cover from '../assets/dazegxdCover.jpg';
import pauseIcon from '../assets/pause.png';
import playIcon from '../assets/play.png';
import backIcon from '../assets/back.png';
import forwardIcon from '../assets/forward.png';

export default function MusicPlayer({isPlaying, togglePlay}) {
    return (
        <div className="musicPlayer">
            <div className="imageDiv">
                <img className="coverImage" src={cover} alt="cover" />
            </div>

            <div className="controls">
                <button className="iconButton">
                    <img src={backIcon} alt="back" />
                </button>

                <button className="iconButton" onClick={togglePlay}>
                    <img src={isPlaying ? pauseIcon : playIcon} alt="play/pause" />
                </button>

                <button className="iconButton">
                    <img src={forwardIcon} alt="forward" />
                </button>
            </div>
        </div>
    );
}