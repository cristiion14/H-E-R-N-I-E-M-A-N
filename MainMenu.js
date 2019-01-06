#pragma strict
var music : AudioSource;
function QuitGame () {
Debug.Log ("exit");
Application.Quit ();
}

function StartGame (level : String) {
Application.LoadLevel ("Level00");
}

function SetGameVolume (vol : float) {
music.volume = vol;
}