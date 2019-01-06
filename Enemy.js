#pragma strict
var deathParticles : Transform;
var player : Rigidbody;
var bounceAmmount = 10f;
var enemyAnim : Animator;
var colliders : Transform;
var deathSound : AudioSource; 
private var centerAnim : Animator;

function Awake() {
	centerAnim = transform.GetComponent ("Animator") as Animator; 
}
function Die () {
player.GetComponent.<Rigidbody>().velocity.y = bounceAmmount;
Instantiate (deathParticles, enemyAnim.transform.position, enemyAnim.transform.rotation);
deathSound.Play(); 
enemyAnim.SetTrigger ("Die");
centerAnim.SetTrigger ("Stop");
Destroy (colliders.gameObject);
//Destroy (gameObject);
}