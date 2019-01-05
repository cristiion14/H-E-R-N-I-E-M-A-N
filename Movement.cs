using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Movement : MonoBehaviour {

    public float speed = 10f;
    public AudioClip chomp1;
    public AudioClip chomp2;
    public Vector2 orientation;
    public Node currentNode, previousNode, targetNode;
    public Sprite idleSprite;
    
    private int palletsConsumed=0;
    private bool playedChomp1 = false;
    private AudioSource audioS;

    private Vector2 direction = Vector2.zero;
    private Vector2 nextDirection;
    private Node startingPos;

    // Use this for initialization
    void Start() {
        audioS = transform.GetComponent<AudioSource>();

        Node node = GetNodeAtPosition(transform.localPosition);
        if(node != null)
        {
            currentNode = node;
            Debug.Log("Current Node is: "+ currentNode);
        }

        direction = Vector2.left;
        orientation = Vector2.left;
        ChangePosition(direction);

    }

    public void Restart()
    {
        //reset PacMan pos
        transform.position = startingPos.transform.position;

        currentNode = startingPos;

        direction = Vector2.left;
        orientation = Vector2.left;
        nextDirection = Vector2.left;

        ChangePosition(direction);
    }

    // Update is called once per frame
    void Update()
    {
        //-coment out to show score

        //Debug.Log("SCORE: " + GameObject.Find("GameManager").GetComponent<GameBoard>().score);
        CheckInput();
        MovePlayer();
        UpdateOrientation();
        UpdateAnimState();
        ConsumePellet();
    }

    void PlayChompSound()
    {
        if(playedChomp1)
        {
            // play chomp2, set playedChomp1 to false
            audioS.PlayOneShot(chomp2);
            playedChomp1 = false;
        }
        else
        {
            // play chomp1, set playedChomp1 to true
            audioS.PlayOneShot(chomp1);
            playedChomp1 = true;
        }
    }

    void CheckInput()
    {
        if (Input.GetKeyDown(KeyCode.LeftArrow))
        {
            ChangePosition(Vector2.left);
        }

       else if (Input.GetKeyDown(KeyCode.RightArrow))
        {
            ChangePosition(Vector2.right);
        }

       else if (Input.GetKeyDown(KeyCode.UpArrow))
        {
            ChangePosition(Vector2.up);
        }

       else if (Input.GetKeyDown(KeyCode.DownArrow))
        {
            ChangePosition(Vector2.down);
        }
    }

    void ChangePosition(Vector2 dir)
    {
        if(dir!=direction)
            nextDirection = dir;

        if (currentNode != null)
        {
            Node moveToNode = CanMove(dir);

            if(moveToNode!=null)
            {
                direction = dir;
                targetNode = moveToNode;
                previousNode = currentNode;
                currentNode = null;

            }
        }

    }

    void MovePlayer()
    {
        if(nextDirection==direction*-1)
        {
            direction *= -1;
            Node tempNode = targetNode;
            targetNode = previousNode;
            previousNode = tempNode;
        }

        if (targetNode != currentNode && targetNode != null)
        {
            if (OverShotTarget())
            {
                currentNode = targetNode;
                transform.localPosition = currentNode.transform.position;

                GameObject otherPortal = GetPortal(currentNode.transform.position);
                if(otherPortal!=null)
                {
                    transform.localPosition = otherPortal.transform.position;
                    currentNode = otherPortal.GetComponent<Node>();
                }

                Node moveToNode = CanMove(nextDirection);

                if (moveToNode != null)
                    direction = nextDirection;

                if (moveToNode == null)
                    moveToNode = CanMove(direction);

                if (moveToNode != null)
                {
                    targetNode = moveToNode;
                    previousNode = currentNode;
                    currentNode = null;
                }
                else
                {
                    direction = Vector2.zero;
                }
            }
            else
            {
                transform.localPosition += (Vector3)(direction * speed) * Time.deltaTime;
            }
        }

    }

    void MoveToNode(Vector2 dir)
    {
        Node moveToNode = CanMove(dir);
        if(moveToNode!= null)
        {
            transform.localPosition = moveToNode.transform.position;
            currentNode = moveToNode;
        }
    }

    void UpdateOrientation()
    {
        orientation = Vector2.left;
        if(direction==Vector2.left)
        {
            transform.localScale = new Vector3(-1, 1, 1);
            transform.localRotation = Quaternion.Euler(0, 0, 0);
        }

       else if (direction == Vector2.right)
        {
            orientation = Vector2.right;
            transform.localScale = new Vector3(1, 1, 1);
            transform.localRotation = Quaternion.Euler(0, 0, 0);
        }

        else if (direction == Vector2.up)
        {
            orientation = Vector2.up;
            transform.localScale = new Vector3(1, 1, 1);
            transform.localRotation = Quaternion.Euler(0, 0, 90);
        }

       else if (direction == Vector2.down)
        {
            orientation = Vector2.down;
            transform.localScale = new Vector3(1, 1, 1);
            transform.localRotation = Quaternion.Euler(0, 0, 270);
        }
    }
    //Animatia Defauld pt PacMan
    void UpdateAnimState()
    {
        if(direction==Vector2.zero)
        {
            GetComponent<Animator>().enabled = false;
            GetComponent<SpriteRenderer>().sprite = idleSprite;
        }
        else
        {
            GetComponent<Animator>().enabled = true;
        }
    }

    void ConsumePellet()
    {
        GameObject obj = GetTileAtPosition(transform.position);

        if(obj!=null)
        {
            Tile tile = obj.GetComponent<Tile>();

            if(tile!=null)
            {
                if(!tile.didConsume&&(tile.isPallet || tile.isSuperPallet))
                {
                    obj.GetComponent<SpriteRenderer>().enabled = false;
                    tile.didConsume = true;
                    GameObject.Find("GameManager").GetComponent<GameBoard>().score += 1;
                    palletsConsumed++;
                    PlayChompSound();

                    if (tile.isSuperPallet)
                    {
                        //-create an array full of ghosts
                        GameObject[] ghosts = GameObject.FindGameObjectsWithTag("Ghost");
                        //-call start frightend mode for every ghost
                        foreach (GameObject go in ghosts)
                        {
                            go.GetComponent<Ghost>().StartFrightendMode();
                        }
                    }
                }
            }
        }
    }

    Node CanMove(Vector2 dir)
    {
        Node moveToNode = null;
        for(int i = 0; i< currentNode.neighbours.Length; i++)
        {
            if((currentNode.validDirections[i] - dir).magnitude <= Vector2.kEpsilon+0.1)
            {
                moveToNode = currentNode.neighbours[i];

            }
        }
        return moveToNode;
    }

    GameObject GetTileAtPosition(Vector2 pos)
    {
        int tileX = Mathf.RoundToInt(pos.x);
        int tileY = Mathf.RoundToInt(pos.y);

        GameObject tile = GameObject.Find("GameManager").GetComponent<GameBoard>().board[tileX, tileY];
        if (tile != null)
            return tile;

        return null;
    }

    Node GetNodeAtPosition(Vector2 pos)
    {
        GameObject tile = GameObject.Find("GameManager").GetComponent<GameBoard>().board[(int)pos.x, (int)pos.y];
        if(tile != null)
        {
            return tile.GetComponent<Node>();
        }
        return null;
    }

    bool OverShotTarget()
    {
        float nodeToTarget = LengthFromNode(targetNode.transform.position);
        float nodeToSelf = LengthFromNode(transform.localPosition);

        return nodeToSelf > nodeToTarget;
    }

    float LengthFromNode(Vector2 targetPosition)
    {
        Vector2 vec = targetPosition - (Vector2)previousNode.transform.position;
        return vec.sqrMagnitude;
    }

    GameObject GetPortal(Vector2 position)
    {
        GameObject tile = GameObject.Find("GameManager").GetComponent<GameBoard>().board[(int)position.x, (int)position.y];

        if(tile!=null)
        {
            if (tile.GetComponent<Tile>() != null)
            {
                if (tile.GetComponent<Tile>().isPortal)
                {
                    GameObject otherPortal = tile.GetComponent<Tile>().portalReciever;
                    return otherPortal;
                }
            }
        }

        return null;

    }

}
