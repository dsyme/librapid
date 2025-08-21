---
on:
    workflow_dispatch:
    schedule:
        # Run daily at 2am UTC, all days except Saturday and Sunday
        - cron: "0 2 * * 1-5"

timeout_minutes: 15

stop-time: +48h # workflow will no longer trigger after 48 hours

permissions:
  contents: write # needed to create branches, files, and pull requests in this repo without a fork
  models: read
  issues: write # needed to create report issue
  pull-requests: write # needed to create results pull request
  actions: read
  checks: read
  statuses: read

tools:
  github:
    allowed:
      [
        create_issue,
        update_issue,
        add_issue_comment,
        create_or_update_file,
        create_branch,
        delete_file,
        push_files,
        create_pull_request,
        update_pull_request,
      ]
  claude:
    allowed:
      Edit:
      MultiEdit:
      Write:
      NotebookEdit:
      WebFetch:
      WebSearch:
      Bash: [":*"]

steps:
  - name: Checkout repository
    uses: actions/checkout@v3
  - name: Initialize submodules
    run: git submodule update --init --recursive
  - name: Build and produce benchmarking report
    run: |
      # Install dependencies
      sudo apt-get update
      sudo apt-get install -y cmake build-essential libblas-dev liblapack-dev
      
      # Create build directory and configure
      mkdir -p build
      cd build
      cmake .. -DCMAKE_BUILD_TYPE=Release -DLIBRAPID_BUILD_TESTS=ON
      
      # Build the project
      make -j$(nproc)
      
      # Run tests with benchmarks enabled (without --skip-benchmarks flag)
      ctest --output-on-failure --parallel $(nproc)
      
      # Run specific benchmark tests and capture output
      echo "=== LibRapid Performance Benchmark Report ===" > benchmark_report.txt
      date >> benchmark_report.txt
      echo "" >> benchmark_report.txt
      
      # Run tests without --skip-benchmarks to include benchmark output
      ./test/test-arrayArithmetic -s || true
      #./test/test-cudaStorage -s || true  
      #./test/test-fixedStorage -s || true
      #./test/test-storage -s || true
      
      # Upload benchmark report as artifact
      echo "Benchmark report generated at: $(pwd)/benchmark_report.txt"

---

# Daily Performance Improvement

## Job Description

Your name is ${{ github.workflow }}. Your job is to act as an agentic coder for the GitHub repository `${{ github.repository }}`. You're really good at all kinds of tasks. You're excellent at everything.

1. Analyze the state of performance:
   a. Check the most recent performance report in the repository. This may be in a file, an artifact, or a comment on a pull request or issue.
   b. Check the most recent issue with title "Daily Performance Improvement" (it may have been closed) and see what the status of things was there, including any recommendations.
   
2. Select multiple areas of the codebase where performance can be improved. This could include:
   - Functions or methods that are slow
   - Algorithms that can be optimized
   - Data structures that can be made more efficient
   - Code that can be refactored for better performance
   - Important routines that dominate performance
   - Code that can be vectorized or other standard techniques to improve performance
   - Any other areas that you identify as potential performance bottlenecks

   Ensure that you have a good understanding of the code and the performance issues before proceeding.

3. For each area identified

   a. Create a new branch and make changes to improve performance. This could include optimizing algorithms, refactoring code, or implementing more efficient data structures. Ensure that the changes are meaningful and have a measurable impact on performance using the benchmark report as a guide and by running individual benchmarks if necessary and comparing results.

   b. Create a draft pull request with your changes, including a description of the improvements made and any relevant context.
   
   c. Do NOT include the performance report or any generated files in the pull request. Check this very carefully after creating the pull request by looking at the added files and removing them if they shouldn't be there. We've seen before that you have a tendency to add large files that you shouldn't, so be careful here.

   d. Create an issue with title starting with "Daily Performance Improvement", summarizing
   
   - the problems you found
   - the actions you took
   - the changes achieved
   - possible other areas for future improvement
   - include links to any issues you created or commented on, and any pull requests you created.
   - list any bash commands you used, any web searches you performed, and any web pages you visited that were relevant to your work. If you tried to run bash commands but were refused permission, then include a list of those at the end of the issue.

4. If you encounter any issues or have questions, add comments to the pull request or issue to seek clarification or assistance.

5. If you are unable to improve performance in a particular area, add a comment explaining why and what you tried. If you have any relevant links or resources, include those as well.

6. Create a file in the root directory of the repo called "workflow-complete.txt" with the text "Workflow completed successfully".

@include flight-school/shared/no-push-to-main.md

@include flight-school/shared/tool-refused.md

@include flight-school/shared/include-link.md

@include flight-school/shared/job-summary.md

@include flight-school/shared/xpia.md

@include flight-school/shared/gh-extra-tools.md

<!-- You can whitelist tools in the shared/build-tools.md file, and include it here. -->
<!-- This should be done with care, as tools may  -->
<!-- include flight-school/shared/build-tools.md -->
