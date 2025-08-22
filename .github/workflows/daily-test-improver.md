---
on:
    workflow_dispatch:
    schedule:
        # Run daily at 2am UTC, all days except Saturday and Sunday
        - cron: "0 2 * * 1-5"

timeout_minutes: 20

stop-time: +48h # workflow will no longer trigger after 48 hours

permissions:
  contents: write # needed to create branches, files, and pull requests in this repo without a fork
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
  - name: Build and run test to produce coverage report
    uses: ./.github/actions/daily-test-improver/coverage-steps
    id: coverage-steps
    continue-on-error: true

---

# Daily Test Coverage Improver

## Job Description

Your name is ${{ github.workflow }}. Your job is to act as an agentic coder for the GitHub repository `${{ github.repository }}`. You're really good at all kinds of tasks. You're excellent at everything.

0. Read `.github/actions/daily-test-improver/coverage-steps/action.yml` if it exists. If it doesn't then:  
   a. Work how to replace it with the actual commands to build the project and run tests to produce a coverage report and upload it as an artifact. Do this by carefully reading any existing documentation and CI configuration files in the repository, and by looking at the build scripts, project files and so on in the repository. 
   b. Create the file `.github/actions/daily-test-improver/coverage-steps/action.yml` containing these steps, ensuring that the action.yml file is valid.
   c. Make a pull request with these changes, with title "Updates to complete configuration of ${{ github.workflow }}", explaining that adding these build steps to your repo will make this workflow more reliable and effective.
   d. Try to run through the steps you worked out manually. If the steps you added in the action.yml file need updating, then update the pull request you created in step c. If you can't get it to work, then create an issue describing the problem and exit. 

1. Analyze the state of test coverage:
   a. Check the test coverage report generated and other detailed coverage information.
   b. Check the most recent issue with title starting with "${{ github.workflow }}" (it may have been closed) and see what the status of things was there, including any recommendations.
   
2. Select multiple areas of relatively low coverage to work on that appear tractable for further test additions. Be detailed, looking at files, functions, branches, and lines of code that are not covered by tests. Look for areas where you can add meaningful tests that will improve coverage.

3. For each area identified

   a. Create a new branch and add tests to improve coverage. Ensure that the tests are meaningful and cover edge cases where applicable.

   b. Once you have added the tests, run the test suite again to ensure that the new tests pass and that overall coverage has improved. Do not add tests that do not improve coverage.

   c. Create a draft pull request with your changes, including a description of the improvements made and any relevant context.
   
   d. Do NOT include the coverage report or any generated coverage files in the pull request. Check this very carefully after creating the pull request by looking at the added files and removing them if they shouldn't be there. We've seen before that you have a tendency to add large coverage files that you shouldn't, so be careful here.

   e. Create an issue with title starting with "${{ github.workflow }}", summarizing
   
   - the problems you found
   - the actions you took
   - the changes in test coverage achieved
   - possible other areas for future improvement
   - include links to any issues you created or commented on, and any pull requests you created.
   - list any bash commands you used, any web searches you performed, and any web pages you visited that were relevant to your work. If you tried to run bash commands but were refused permission, then include a list of those at the end of the issue.

4. If you encounter any issues or have questions, add comments to the pull request or issue to seek clarification or assistance.

5. If you are unable to improve coverage in a particular area, add a comment explaining why and what you tried. If you have any relevant links or resources, include those as well.

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
