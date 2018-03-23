# mini-project-tracking

#### Guide for basic github command line (good enough for this project purposes)
---


###### Basic push and pull. first time cloning to your computer.
1. clone this repository. (repository link can be copied on green button that says "Clone or download").
    ```
    git clone <repository link>
    ```
2. enter the local directory of the repository.
    ```
    cd <repository name or file name>
    ```
3. work on your project.
4. to check status of your project (optional), do
    ```
    git status
    ```
    it will show you some file name in red. it's red because you did your work and made improvements to the associated file.
5. update your work,
    ```
    git add .
    ```
6. commit your new update, with a memo
    ```
    git commit -m "your memo about your update. always make a good memo"
    ```

7. check the status of your project again (optional).
    ```
    git status
    ```
    now all the files that were red became green on your terminal.
7.  check your available remote access to branches.
    ```
    git remote -v
    ```
8.  push to the master branch.
    ```
    git push origin master
    ```
    then log in using your github credentials.

example summary,
```
git clone https://...
cd my_project_file_name
git status                //checking project status

                --working on project, make changes--

git add .
git status                //checking project status
git remote -v             //checking the remote access
git push origin master    //pushing into master
```
---

###### Always work on a branch, not master.
-  to create a branch.
    ```
    git checkout -b <branch name>
    ```
    or
    ```
    git checkout branch <branch name>
    ```
- to check the available branch on the repository.

  to show both remote and local branches.
    ```
    git branch -a
    ```
    to show remote branches only.
    ```
    git branch -r
    ```
- to go to an existing branch
    ```
    git checkout <branch name>
    ```
- pushing to branch, NOT MASTER
    ```
    git add .
    git status
    git commit -m "I fixed the bugs on my branch! :D"
    git push origin mybranch_name
    ```

example on creating a branch
```
--currently on master--
git checkout -b myPersonal_branch
git status
--working on my branch--
git add . 
git status
git commit -m "I fixed the bugs on my branch! :D"
git remote -v
git push origin myPersonal_branch

```

---
