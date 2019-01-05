using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Ghost : MonoBehaviour {

    public float moveSpeed = 5.9f;
    public float normalMoveSpeed = 5.9f;
    public float frightendModeSpeed = 2.9f;
    public float consumedMoveSpeed = 15f;

    public float ghostReleaseTimer = 0f;
    public int pinkyReleaseTimer = 5;           //after 5 sec, release pinky from the house
    public int inkyReleaseTimer = 14;
    public int clydeReleaseTimer = 21;

    public int frightendModeDuration = 10;
    public int startBlinkingAt = 7;     

    public bool isInGhostHouse = false;

    public Node startingPosition;
    public Node homeNode;
    public Node ghostHouse;

    public int scatterModeTimer1 = 7;
    public int chaseModeTimer1 = 20;
    public int scatterModeTimer2 = 7;
    public int chaseModeTimer2 = 20;
    public int scatterModetimer3 = 5;
    public int chaseModeTimer3 = 20;
    public int scatterModeTimer4 = 5;


    public Sprite eyesUp, eyesDown, eyesLeft, eyesRight;


    public RuntimeAnimatorController ghostUp, ghostDown, ghostRight, ghostLeft, ghostWhite, ghostBlue;

    private int modeChangeIteration = 1;
    private float modeChangeTimer = 0;
    private float previousMoveSpeed;

    private AudioSource backroundAudio;

    public float frightenedModetimer = 0;
    public float blinkTimer = 0;

    public bool frightenedModeIsWhite = false;

    public enum Mode
    {
        Chase,
        Scatter,
        Frightend,
        Consumed
    }

    public enum GhostType
    {
        Red,
        Pink,
        Blue,
        Orange
    }
    public GhostType ghostType = GhostType.Red;

    Mode currentMode = Mode.Scatter;
    Mode previousMode;

    private GameObject pacMan;
    private Node currentNode, targetNode, previousNode;
    private Vector2 direction, nextDirection;

    // Use this for initialization
    void Start() {

        backroundAudio = GameObject.Find("GameManager").transform.GetComponent<AudioSource>();
        pacMan = GameObject.FindGameObjectWithTag("PacMan");
        Node node = GetNodeAtPosition(transform.localPosition);

        
        if (node!= null)
        {
            currentNode = node;
        }

        if(isInGhostHouse)
        {
            direction = Vector2.up;
            targetNode = currentNode.neighbours[0];
        }
        else
        {
            direction = Vector2.left;
            targetNode = ChooseNextNode();
        }
     

        previousNode = currentNode;

        UpdateAnimatorController();

    }

    // Update is called once per frame
    void Update() {

        ModeUpdate();
        Move();
        ReleaseGhosts();
        CheckCollision();
    }

    void CheckIsInGhostHouse()
    {
        if(currentMode==Mode.Consumed)
        {

        }
    }

    void CheckCollision()
    {
        //check if one rectangle overlaps another rectangle
        Rect ghostRect = new Rect(transform.position, transform.GetComponent<SpriteRenderer>().sprite.bounds.size / 4);     //divide the rect by 4 for creating a smaller collision detection
        Rect pacManRect = new Rect(pacMan.transform.position, pacMan.transform.GetComponent<SpriteRenderer>().sprite.bounds.size / 4);

        if(ghostRect.Overlaps(pacManRect))
        {
            //return true or false if the players are overlapping
            if (currentMode == Mode.Frightend)
            {
                Consumed();
            }
            else
            {
                //PacMan should die
            }
        }

    }

    void Consumed()
    {
        currentMode = Mode.Consumed;
        previousMoveSpeed = moveSpeed;
        moveSpeed = consumedMoveSpeed;
        UpdateAnimatorController();
    }

    void UpdateAnimatorController()
    {
        if (currentMode != Mode.Frightend && currentMode != Mode.Consumed)
        {
            if (direction == Vector2.up)
            {
                transform.GetComponent<Animator>().runtimeAnimatorController = ghostUp;
            }
            else if (direction == Vector2.down)
            {
                transform.GetComponent<Animator>().runtimeAnimatorController = ghostDown;
            }
            else if (direction == Vector2.left)
            {
                transform.GetComponent<Animator>().runtimeAnimatorController = ghostLeft;
            }
            else if (direction == Vector2.right)
            {
                transform.GetComponent<Animator>().runtimeAnimatorController = ghostRight;
            }
            else
            {
                transform.GetComponent<Animator>().runtimeAnimatorController = ghostLeft;
            }
        }
        else if(currentMode==Mode.Frightend)
        {
            transform.GetComponent<Animator>().runtimeAnimatorController = ghostBlue;
        }
        else if(currentMode==Mode.Consumed)
        {
            //set the animator controller to null
            transform.GetComponent<Animator>().runtimeAnimatorController = null;
            //update the sprite renderer to the eyes instead of ghost
            if(direction==Vector2.up)
            {
                transform.GetComponent<SpriteRenderer>().sprite = eyesUp;
            }
            else if(direction==Vector2.left)
            {
                transform.GetComponent<SpriteRenderer>().sprite = eyesLeft;
            }
            else if(direction==Vector2.right)
            {
                transform.GetComponent<SpriteRenderer>().sprite = eyesRight;
            }
            else if(direction==Vector2.down)
            {
                transform.GetComponent<SpriteRenderer>().sprite = eyesDown;
            }
        }
    }

    void Move()
    {
        if(targetNode!=currentNode&&targetNode!=null&& !isInGhostHouse)
        {
            if(OverShotTarget())
            {
                currentNode = targetNode;
                transform.localPosition = currentNode.transform.position ;
                GameObject otherPortal = GetPortal(currentNode.transform.position);
                if(otherPortal!=null)
                {
                    transform.localPosition = otherPortal.transform.transform.position;
                    currentNode = otherPortal.GetComponent<Node>();
                }
                targetNode = ChooseNextNode();
                previousNode = currentNode;
                currentNode = null;

                UpdateAnimatorController();
            }
            else
            {
                transform.localPosition += (Vector3)direction * moveSpeed * Time.deltaTime;
            }
        }
    }
    void ModeUpdate()
    {
        if (currentMode != Mode.Frightend)
        {
            modeChangeTimer += Time.deltaTime;
            if (modeChangeIteration == 1)
            {
                if (currentMode == Mode.Scatter && modeChangeTimer > scatterModeTimer1)
                {
                    ChangeMode(Mode.Chase);
                    modeChangeTimer = 0;
                }

                if (currentMode == Mode.Chase && modeChangeTimer > chaseModeTimer1)
                {
                    modeChangeIteration = 2;
                    ChangeMode(Mode.Scatter);
                    modeChangeTimer = 0;
                }
            }
            else if (modeChangeIteration == 2)
            {
                if (currentMode == Mode.Scatter && modeChangeTimer > scatterModeTimer2)
                {
                    ChangeMode(Mode.Chase);
                    modeChangeTimer = 0;
                }
                if (currentMode == Mode.Chase && modeChangeTimer > chaseModeTimer2)
                {
                    modeChangeIteration = 3;
                    ChangeMode(Mode.Scatter);
                    modeChangeTimer = 0;
                }

            }
            else if (modeChangeIteration == 3)
            {
                if (currentMode == Mode.Scatter && modeChangeTimer > scatterModetimer3)
                {
                    ChangeMode(Mode.Chase);
                    modeChangeTimer = 0;
                }
                if (currentMode == Mode.Chase && modeChangeTimer > chaseModeTimer3)
                {
                    modeChangeIteration = 4;
                    ChangeMode(Mode.Scatter);
                    modeChangeTimer = 0;
                }

            }
        }
        else if (modeChangeIteration == 4)
        {
            if (currentMode == Mode.Scatter && modeChangeTimer > scatterModeTimer4)
            {
                ChangeMode(Mode.Chase);
                modeChangeTimer = 0;
            }
        }
        else if (currentMode == Mode.Frightend)
        {
            frightenedModetimer += Time.deltaTime;

            if (frightenedModetimer >= frightendModeDuration)
            {
                backroundAudio.clip = GameObject.Find("GameManager").transform.GetComponent<GameBoard>().backroundAudioNoramal;
                backroundAudio.Play();

                frightenedModetimer = 0;
                ChangeMode(previousMode);
            }

            if (frightenedModetimer >= startBlinkingAt)
            {
                blinkTimer += Time.deltaTime;

                if (blinkTimer >= 0.1f)
                {
                    blinkTimer = 0f;
                    if (frightenedModeIsWhite)
                    {
                        transform.GetComponent<Animator>().runtimeAnimatorController = ghostBlue;
                        frightenedModeIsWhite = false;
                    }
                    else
                    {
                        transform.GetComponent<Animator>().runtimeAnimatorController = ghostWhite;
                        frightenedModeIsWhite = true;
                    }
                }
            }
        }
     }
   
    
    void ChangeMode(Mode m)
    {

        if(currentMode==Mode.Frightend)
        {
            moveSpeed = previousMoveSpeed;
            
        }
        if(m==Mode.Frightend)
        {
            previousMoveSpeed = moveSpeed;
            moveSpeed = frightendModeSpeed;
           
        }
        if (currentMode != m)
        {
            previousMode = currentMode;
            currentMode = m;
        }

        UpdateAnimatorController();
    }

    public void StartFrightendMode()
    {
        frightenedModetimer = 0;
        backroundAudio.clip = GameObject.Find("GameManager").transform.GetComponent<GameBoard>().backroundAudioFrightened;
        backroundAudio.Play();
        //-change the mode to frightend
        ChangeMode(Mode.Frightend);

    }

    //get the target tile for the ghosts
    Vector2 GetRedGhostTarget()
    {
        Vector2 PacManPos = pacMan.transform.localPosition;
        Vector2 targetTile = new Vector2(Mathf.RoundToInt(PacManPos.x), Mathf.RoundToInt(PacManPos.y));

        return targetTile;
    }

    Vector2 GetPinkGhostTarget()
    {
        // four tiles ahead Pac-Man
        // taking accoung position and orientation
        Vector2 PacManPos = pacMan.transform.localPosition;
        Vector2 PacManOrientation = pacMan.GetComponent<Movement>().orientation;

        int pacManPosX = Mathf.RoundToInt(PacManPos.x);
        int pacManPosY = Mathf.RoundToInt(PacManPos.y);

        Vector2 pacManTile = new Vector2(pacManPosX, pacManPosY);
        Vector2 targetTile = pacManTile + (4 * PacManOrientation);


        return targetTile;
    }
    Vector2 GetBlueGhostTarget()
    {
        //position 2 tiles in front of PacMan
        //Draw vector from Blinky to that pos
        //Double the length of the vector
        Vector2 PacManPos = pacMan.transform.localPosition;
        Vector2 PacManOrientation = pacMan.GetComponent<Movement>().orientation;

        int pacManPosX = Mathf.RoundToInt(PacManPos.x);
        int pacManPosY = Mathf.RoundToInt(PacManPos.y);

        Vector2 pacManTile = new Vector2(pacManPosX, pacManPosY);
        Vector2 targetTile = pacManTile + (2 * PacManOrientation);

        //temp blinky pos
        Vector2 tempBlinkyPos = GameObject.Find("blinky").transform.localPosition;
        int blinkyPosX = Mathf.RoundToInt(tempBlinkyPos.x);
        int blinkyPosY = Mathf.RoundToInt(tempBlinkyPos.y);

        Vector2 blinkyTile = new Vector2(blinkyPosX, blinkyPosY);
        float distance = GetDistance(tempBlinkyPos, targetTile);
        distance *= 2;

        targetTile = new Vector2(tempBlinkyPos.x + distance, tempBlinkyPos.y + distance);

        return targetTile;
    }

    Vector2 GetOrangeGhostTarget()
    {
        //calculates the distance from PacMan
        //if the distance is greater than eight tiles targeting is the same as blinky 
        //if the distance is less than eight tiles then target is his home node, like scatter mode

        Vector2 pacManPos = pacMan.transform.localPosition;

        float distance = GetDistance(transform.localPosition, pacManPos);
        Vector2 targetTile = Vector2.zero;

        if(distance>8)
        {
            targetTile = new Vector2(Mathf.RoundToInt(pacManPos.x), Mathf.RoundToInt(pacManPos.y));
        }
        else if(distance<8)
        {
            targetTile = homeNode.transform.position;
        }

        return targetTile;

    }

    Vector2 GetTargetTile()
    {
        Vector2 targetTile = Vector2.zero;
        if(ghostType==GhostType.Red)
        {
            targetTile = GetRedGhostTarget();
        }
        else if(ghostType==GhostType.Pink)
        {
            targetTile = GetPinkGhostTarget();
        }
        else if(ghostType==GhostType.Blue)
        {
            targetTile = GetBlueGhostTarget();
        }
        else if(ghostType==GhostType.Orange)
        {
            targetTile = GetOrangeGhostTarget();
        }

        return targetTile;
    }

    void ReleasePinky()
    {
        if(ghostType==GhostType.Pink&&isInGhostHouse)
        {
            isInGhostHouse = false;
        }
    }

    void ReleaseBlueGhost()
    {
        if(ghostType==GhostType.Blue&&isInGhostHouse)
        {
            isInGhostHouse = false;
        }
    }

    void ReleaseOrangeGhost()
    {
        if(ghostType==GhostType.Orange&&isInGhostHouse)
        {
            isInGhostHouse = false;
        }
    }

    void ReleaseGhosts()
    {
        ghostReleaseTimer += Time.deltaTime;
        if(ghostReleaseTimer>pinkyReleaseTimer)
        {
            ReleasePinky();
        }

        if(ghostReleaseTimer>inkyReleaseTimer)
        {
            ReleaseBlueGhost();
        }

        if(ghostReleaseTimer>clydeReleaseTimer)
        {
            ReleaseOrangeGhost();
        }
    }

    Node ChooseNextNode()
    {
        Vector2 targetTile = Vector2.zero;

        if (currentMode == Mode.Chase)
        {
            targetTile = GetTargetTile();
        }
        else if(currentMode==Mode.Scatter)
        {
            targetTile = homeNode.transform.position;
        }
        Node moveToNode = null;

        Node[] foundNodes = new Node[4];
        Vector2[] foundNodesDir = new Vector2[4];

        int nodeCounter = 0;

        for(int i=0;i<currentNode.neighbours.Length;i++)
        {
            if(currentNode.validDirections[i] != direction * -1)
            {
                foundNodes[nodeCounter] = currentNode.neighbours[i];
                foundNodesDir[nodeCounter] = currentNode.validDirections[i];
                nodeCounter++;
            }
        }

        if(foundNodes.Length==1)
        {
            moveToNode = foundNodes[0];
            direction = foundNodesDir[0];
        }
        if(foundNodes.Length>1)
        {
            float leastDistance = 100000f;
            //find the shortest distance
            for(int i=0; i<foundNodes.Length;i++)
            {
                if(foundNodesDir[i] !=Vector2.zero)
                {
                    float distance = GetDistance(foundNodes[i].transform.position, targetTile);
                    if(distance<leastDistance)
                    {
                        leastDistance = distance;
                        moveToNode = foundNodes[i];
                        direction = foundNodesDir[i];
                    }
                }
            }
        }
        return moveToNode;
    }

    Node GetNodeAtPosition(Vector2 pos)
    {
        GameObject tile = GameObject.Find("GameManager").GetComponent<GameBoard>().board[(int)pos.x, (int)pos.y];
        if (tile != null)
        {
            if (tile.GetComponent<Node>() != null)
            {
                return tile.GetComponent<Node>();
            }
        }
 
        return null;
    }

    GameObject GetTileAtPosition(Vector2 pos)
    {
        int tileX = Mathf.RoundToInt(pos.x);
        int tileY = Mathf.RoundToInt(pos.y);

        GameObject tile = GameObject.Find("GameManager").transform.GetComponent<GameBoard>().board[tileX, tileY];
        if(tile!=null)
        {
            return tile;
        }
        else
        {
            return null;
        }
    }

    GameObject GetPortal(Vector2 pos)
    {
        GameObject tile = GameObject.Find("GameManager").GetComponent<GameBoard>().board[(int)pos.x, (int)pos.y];

        if (tile != null)
        {
            if (tile.GetComponent<Tile>().isPortal)
            {
                GameObject otherPortal = tile.GetComponent<Tile>().portalReciever;
                return otherPortal;
            }
        }
        return null;
    }
    float LengthFromNode(Vector2 targetPosition)
    {
 
        Vector2 vec = targetPosition - (Vector2)previousNode.transform.position;
        return vec.sqrMagnitude;
    }

    bool OverShotTarget()
    {
        float nodeToTarget = LengthFromNode(targetNode.transform.position);
        float nodeToSelf = LengthFromNode(transform.localPosition);
        return nodeToSelf > nodeToTarget;
    }

    float GetDistance(Vector2 posA, Vector2 posB)
    {
        float dx = posA.x - posB.x;
        float dy = posA.y - posB.y;

        float distance = Mathf.Sqrt(dx * dx + dy * dy);
        return distance;
    }
}
