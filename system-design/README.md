# Robotics system design, from level 1 to the top

This folder teaches you to design a robot's software as a whole, not just to write one
algorithm. It starts with no assumptions: level 1 has no code and no jargon. Each level builds
on the one before. Read them in order.

| Level | File | What you can do after it |
|---|---|---|
| 0 | [The tree](00-the-tree.md) | The whole subject as one tree; redraw it weekly from memory |
| 1 | [What a system is](01-level-1-what-is-a-system.md) | Draw any robot as boxes and arrows, label the arrows, name a trade-off |
| 2 | [The robot software stack](02-level-2-the-robot-software-stack.md) | Name every box in a real robot's stack, explain topics/services/actions, frames and time |
| 3 | [Timing, reliability and safety](03-level-3-timing-reliability-safety.md) | Build a latency budget, handle failures, draw a state machine, test a robot system |
| 4 | [Fleets, cloud and scale](04-level-4-fleets-cloud-and-scale.md) | Design for 200 robots: what runs where, traffic, updates, monitoring, safety cases |
| 5 | [Design questions, worked](05-level-5-design-questions-worked.md) | Take "design a robot that does X" to a full design in 40 minutes, six worked examples |

## How to study this

- One level per sitting. Draw every diagram yourself on paper before reading the one in the
  text. Drawing is the skill; reading is not.
- Do the "Try it" exercises. Level 5 contains the answers to the level 1 exercises.
- After level 3, go back to the coding tutorial and notice which box each algorithm lives in:
  BFS and A* in [tutorial/03](../tutorial/03-grids-bfs-dfs.md) are the planner box, the
  Kalman filter and PID in [tutorial/06](../tutorial/06-robotics.md) are the state estimation
  and control boxes.

## The one idea

Every robot is **sense → think → act**. Designing is drawing those boxes, labelling what flows
between them and how often, and choosing trade-offs on purpose. The hard problems live on the
arrows: how fresh the data is, how much of it there is, and what happens when it stops.
