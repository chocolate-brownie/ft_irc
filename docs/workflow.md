### 🐙 Git: Using Branching Workflow

We have a team of three. If you all push to the `main` (or `master`) branch, you will overwrite each other's work and create a nightmare of merge conflicts. 🤯

**The Practice:** 🌿 **Feature Branching** + 💬 **Pull Requests**.

**Analogy:** Think of your `main` branch as the 🚢 **"release" version** of your server. It should *only* contain code that *works* and has been *approved* by the team.

Here is the workflow. For **every** new task (e.g., "Implement the NICK command"):

1.  🔄 **Pull:** `git pull origin main`
    * (Make sure you have the latest code from the "release" branch).
2.  🌿 **Branch:** `git checkout -b feature/implement-nick`
    * (Create your own "sandbox" branch).
3.  💻 **Code & Commit:** Do your work.
    * Commit *locally* as much as you want.
4.  📤 **Push:** `git push origin feature/implement-nick`
    * (Push your *branch* up to the shared repository).
5.  🙋 **Pull Request (PR):** Go to your Git repo's website (GitHub, GitLab, etc.).
    * Open a "Pull Request" to merge `feature/implement-nick` into `main`.
6.  👀 **Review:** This is the most important step.
    * **A teammate must read your code.** They check for bugs, style, and understanding. They can "request changes."
7.  ✅ **Approve & Merge:** Once it's approved, the branch is merged into `main`.

---
### 🚀 Why We Should All Use `.clang-format`

To make our `ft_irc` project smoother and easier for everyone, we're going to use this `.clang-format` file.

Instead of us wasting time worrying about different coding styles (like spaces, tabs, or brace placement), this tool automatically cleans up our code to match one single style for all of us

**The benefits are huge:**

* **Easy Readability:** The *entire* codebase will look like one person wrote it. This makes it 100x easier to read, understand, and debug each other's work.
* **Focus on What Matters:** We can focus our brainpower on our server's *logic* (the hard part!), not on manual formatting.
* **It's Automatic:** You don't even have to *think* about it. We can set up our code editors to do it every single time we hit "Save."

The `.clang-format` file we have is already set up for us. It's based on the LLVM style and enforces C++03 (C++98) rules , 4-space indents, and a `ColumnLimit` of 100, which is perfect for our project.

``find src includes -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i``

### 🔧 How to Use It in VS Code (in 3 steps)

1.  **Install the Extension:**
    * Go to the Extensions tab (or `Ctrl+Shift+X`).
    * Search for `C/C++`.
    * Install the main "C/C++" extension pack from Microsoft. This includes the `clang-format` tool.

2.  **Enable "Format on Save":**
    * Open your VS Code Settings (click the ⚙️ Gear Icon > `Settings`, or press `Ctrl+,`).
    * In the search bar, type `Format on Save`.
    * Check the box! This will now format your file every time you press `Ctrl+S`.

3.  **Test It!**
    * Make sure the `.clang-format` file is in the root of our project folder.
    * Open one of your `.cpp` files.
    * Mess up the indentation on purpose.
    * Hit **Save (`Ctrl+S`)**.
    * Watch the magic happen as your code instantly snaps into the correct format. ✨

-----
### 📄 What is the `.github/workflows/build.yml` file?

This file is our team's **automated quality checker**.

It's a "GitHub Actions" workflow, which is a set of instructions that GitHub will run for us automatically.

### 🤔 What's the point of it?

Its purpose is to **protect our `main` branch from any code that doesn't compile.**

By using this file with "Branch Protection," we guarantee that:

1.  **No one can accidentally break `main`:** You can't merge a "feature" if it makes the project impossible to build.
2.  **It saves us time:** We don't have to manually pull our teammates' branches to test if they even compile. GitHub does it for us.
3.  **It forces good habits:** It makes us all use the professional workflow of Pull Requests.

### ⚙️ What's inside it? (A Quick Breakdown)

```yaml
# A human-readable name for this check
name: C++ Build

# When to run: On any pull request that wants to merge into 'main'
on:
  pull_request:
    branches: [ main ]

jobs:
  build: # The name of this "job"
    # What to use: A fresh, new "ubuntu-latest" (Linux) computer
    runs-on: ubuntu-latest

    steps: # The list of commands to run
    # Step 1: Get the new code
    - uses: actions/checkout@v3

    # Step 2: Run "make". If this command fails, the whole check fails.
    - name: Build with Make
      run: make
```

In short, it's our "security guard" that makes sure **only working, compilable code** ever gets into our main branch.
