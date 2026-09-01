# Awesome MCP Best Practices ![Awesome](https://cdn.rawgit.com/sindresorhus/awesome/d7305f38d29fed78fa85652e3a63e154dd8e8829/media/badge.svg)

A curated and opinionated list of awesome Model Context Protocol (MCP) best practices as they pertain to building MCP Servers and MCP Clients.

---

## MCP Servers

- 1 MCP Server Tools
  - 1.1 [Tool Naming Standards](#-11-Tool-Naming-Standards)
  - 1.2 [Tool Naming Aliases](#-12-Tool-Naming-Aliases)
  - 1.3 [Tool Description Standards](#-13-tool-description-standards)
  - 1.4 [Avoid Not Found Responses](#-14-avoid-not-found-responses)
- 2 MCP Server Definition
  - 2.1 [Provide Rich Server Instructions](#-21-provide-rich-server-instructions)
- 3 MCP Server Architecture
  -  3.1 [Abstract Server Capabilities](#-31-abstract-server-capabilities)
  -  3.2 [Dont Map 1:1 APIs to Tools](#-32-dont-map-11-apis-to-tools)
- MCP Server Testing
- MCP Server Deployment
  - [Package Your MCP Server as a Docker Container](#-package-your-mcp-server-as-a-docker-container)
- MCP Server Security
  - [Secure MCP Server Code]()
  - [Secure MCP Server Dependencies](#-secure-mcp-server-dependencies)
  - [Avoid descriptive errors]()
- MCP Server Performance
  - [Cache costly Tools]()
- MCP Server Errors and Observability
  - [Graceful timeouts]()
  - 

## MCP Clients

// TBD

---

## 1 MCP Server Tools

### 🔵 1.1 Tool Naming Standards

Use consistent, compatible naming conventions for your MCP Server `Tools` to ensure they can be properly discovered and invoked by MCP Clients.

#### ❌ Avoid These Tool Naming Conventions

- Spaces: `get Npm Package Info`
- Dot notation: `get.Npm.Package.Info`
- Brackets/parentheses: `get(Npm)PackageInfo`

#### ✅ Recommended Tool Naming Conventions

- ✅ camelCase (**preferred**): `getNpmPackageInfo`
- kebab-case: `get-npm-package-info`
- snake_case: `get_npm_package_info`

```javascript
server.tool(
  "getNpmPackageInfo",
  "Get information about an npm package",
  {
    packageName: z.string()
  },
  async ({ packageName }) => {    
    // Implementation details...
    return {
      content: [{ type: "text", text: output }],
    };
  }
);
```

#### 💡 Why It Matters

Using non-standard naming conventions can prevent or disrupt MCP Clients from properly discovering and surfacing your tools to end users. GPT-4o tokenization works best with `camelCase` naming conventions.

---

### 🔵 1.2 Tool Naming Aliases

When the Tool name can be intereted and accessed using different naming conventions, call out aliases in the Tool's description.

#### ❌ Problematic Pattern

For example, a `postMesage` tool name might be too specific:

```javascript
server.tool(
  "postMessage",
  "Post a message under your account",
   () => {}
)
```

The LLM might not invoke the tool if the users ask for "share a social post on Twitter", or "upload this picture to Instagram".

#### ✅ Recommended Practice

Specify aliases and elaborate description for LLMs to better understand when it is required to invoke your tool.

```javascript
server.tool(
  "postMessage",
  "Upload, share, and post messages on social media",
   () => {}
)
```

---

### 🔵 1.3 Tool Description Standards

Even with large content windows, choosing the right tool to call, especially when many tools are exposed, will be a difficult task for an LLM. Providing as much context as possible within the description of tools is necessary and helpful.

#### ❌ Avoid These Tool Description Conventions

Short description such as "Call this function to execute an SQL query"
 
#### ✅ Recommended Tool Description Conventions

- Provide a use-case example reference
- Add necessary notes and nuances relevant to the tool

```javascript
server.tool(
  "runSqlQuery",
  `<use_case>Use this tool to execute a single SQL query against a Postgres database.</use_case>
   <important_notes>
     If you have a temporary branch from a prior step, you MUST:
     1. Pass the branch ID to this tool unless explicitly told otherwise
     2. Tell the user that you ar eusing the temporary branch with ID [branch_id]
   </important_notes>
  `
);
```

#### 💡 Why It Matters

Using non-standard naming conventions can prevent or disrupt MCP Clients from properly discovering and surfacing your tools to end users. GPT-4o tokenization works best with `camelCase` naming conventions.

---

### 🔵 1.4 Avoid Not Found Responses

When implementing search-type tools in your MCP Server, avoid returning explicit "not found" messages even when exact matches aren't available.

#### ❌ Problematic Pattern

```javascript
// Don't do this
if (!exactMatch) {
  return {
    content: [
      { 
        type: "text", 
        text: `Module ${query} not found. Here are all available modules: ${allModules}` 
      }
    ]
  };
}
````

#### ✅ Recommended Practice

```javascript
// Do this instead
return {
  content: [
    { 
      type: "text",
      text: `Here are the available modules that may help with your query: ${relevantModules}`
    }
  ]
};
```

#### 💡 Why It Matters

Let the LLM determine relevance from the data provided rather than prematurely declaring failure in your tool response. LLMs can be overly influenced by negative statements like "not found" causing them to ignore the useful information that follows. By providing relevant data without negative framing, you enable the LLM to process and utilize all available information properly.

#### ⚠️ Important Exception

This approach isn't appropriate for all scenarios. When handling sensitive data (like user information), security and privacy concerns should take precedence over providing alternative data.

---

## 2 MCP Server Definition

### 🔵 2.1 Provide Rich Server Instructions

Provide a rich description of the MCP Server to clearly explain its purpose, how tools, resources and other capabilities should be mapped to workflows and actions. Enhance the description with best practices, and any other information that helps the LLM gain context on how to best use the MCP Server.

#### ✅ Recommended Practice

Example for rich instructions provided to the MCP Server, reference from [Graphlit MCP Server](https://github.com/graphlit/graphlit-mcp-server/blob/main/src/index.ts)

```js
const DEFAULT_INSTRUCTIONS = `
You are provided a set of MCP tools and resources that integrate with the [Graphlit](https://www.graphlit.com) Platform.

To use each of the Graphlit MCP tools, there may be environment variables which are required to be configured in your MCP client. These are described in the description for each tool.
These must be configured in the MCP client YAML or JSON configuration file before you can use the tools. *Do not* set these directly in your Terminal or shell environment.

Graphlit is an LLM-enabled knowledge API platform, which supports these resources:
- project: container for ingested contents, which can be configured with a default workflow
- contents: all ingested files, web pages, messages, etc.; also includes short-term 'memory' contents
- feeds: data connectors which ingest contents
- collections: named groups of contents
- conversations: chat message history of LLM conversation, which uses RAG pipeline for content retrieval
- workflows: how content is handled during the ingestion process
- specifications: LLM configuration presets, used by workflows and conversations

Identifiers for all resources are unique within the Graphlit project, and are formatted as GUIDs.

You have access to one and only one Graphlit project, which can optionally be configured with a workflow to guide the document preparation and entity extraction of ingested content. 
The Graphlit project is non-deletable, but you can create and delete contents, feeds, collections, conversations, specifications and workflows within the project.

You can query the Graphlit project resource for the credits used, LLM tokens used, and the available project quota. By default, credits cost USD$0.10, and are discounted on higher paid tiers.

With this Graphlit MCP Server, you can ingest anything from Slack, Discord, websites, Notion, Google Drive, email, Jira, Linear or GitHub into a Graphlit project - and then search and retrieve relevant knowledge within an MCP client like Cursor, Windsurf or Cline.

Documents (PDF, DOCX, PPTX, etc.) and HTML web pages will be extracted to Markdown upon ingestion. Audio and video files will be transcribed upon ingestion.

## Best Practices:
1. Always look for matching resources before you try to call any tools.
For example, "have i configured any graphlit workflows?", you should check for workflow resources before trying to call any other tools.
2. Don't use 'retrieveSources' to locate contents, when you have already added the contents into a collection. In that case, first retrieve the collection resource, which contains the content resources.
3. Only call the 'configureProject' tool when the user explicitly asks to configure their Graphlit project defaults.
4. Never infer, guess at or hallucinate any URLs. Always retrieve the latest content resources in order to get downloadable URLs.
5. Use 'ingestMemory' to save short-term memories, such as temporary notes or intermediate state for research. Use 'ingestText' to store long-term knowledge, such as Markdown results from research.
6. Always use 'PODSCAN' web search type when searching for podcast episodes, podcast appearances, etc.
7. Prioritize using feeds, rather than 'ingestUrl', when you want to ingest a website. Feeds are more efficient and faster than using 'ingestUrl'.
If you receive a request to ingest a GitHub URL, use the 'ingestGitHubFiles' tool to ingest the repository, rather than using 'ingestUrl'.
Always attempt to use the most-specific tool for the task at hand.

## Short-term vs Long-term Memory:
You can perform scatter-gather operations where you save short-term memories after each workflow step, and then gather relevant memories prior to the moving onto the next step. 
Leverage short-term memories when evaluating the results of a workflow step, and then use long-term memories to store the final results of your workflow.
You can collect memories in collections, and then use the 'queryContents' tool to retrieve the 'memory' contents by the collection. This will help you to keep track of your progress and avoid losing any important information.

If you have any trouble with this Graphlit MCP Server, join our [Discord](https://discord.gg/ygFmfjy3Qx) community for support.
`;

export const server = new McpServer(
  {
    name: "Graphlit MCP Server",
    version: "1.0.0",
  },
  {
    instructions: DEFAULT_INSTRUCTIONS,
  }
);
```


---

## 3 MCP Server Architecture

### 🔵 3.1 Abstract Server Capabilities

Follow an inversion-of-control paradigm to allow your MCP Server capabilities to receive a `server` object and use it to apply `tools`, `resources`, `prompts`, and other capabilities.

#### 💡 Why It Matters

You may not know ahead of time if you will need to build for an STDIO or HTTP transports, nor you don't know which SDK or cloud infrastructure you will be deploying to (e.g: Vercel vs Cloudflare). To allow the underlying MCP Server logic implementation to supplement any of these future concerns, build it in a way that 

// TBD bad pattern

// TBD good pattern

// TBD example code

---

### 🔵 3.2 Dont Map 1:1 APIs to Tools

Avoid mapping your traditional RESTful or GraphQL APIs in a 1:1 mapping with your Tools. This leads to poor experience for LLMs to drive high-level use-case scenarios.

#### ❌ Avoid 1:1 API Mapping to Tools

While mapping your APIs exactly as they are to tools definitions in an MCP server might seem like a quick low-hanging fruit, it introduces these problems and challenges:

- Too many tools (as there are APIs, which often for a given application is counted in tens or more)
- Too many tool calls leads to too many tokens
- Tools become too fine-grained and LLMs need to perform several loops of tool calls to assemble one use-case action
- How do you decide between different API versions and their tool targets?

Case-study: [Notion API to MCP Tools story](https://x.com/NotionAPI/status/1955666043235692622)
 
#### ✅ Recommended Tool Definitions

- Define tools as high-level use-case scenarios that will be driven by LLMs

#### 💡 Why It Matters

You may 

---

## MCP Server Deployment

### 🔵 Package Your MCP Server as a Docker Container

Deploy your MCP Servers as Docker containers to eliminate environment setup challenges and ensure consistent operation across different systems.

#### 💡 Why It Matters

MCP Servers often require specific runtime environments (Node.js, Python) with particular versions and dependencies. Docker abstracts away these requirements, turning complex setup instructions into a simple container run command.

#### 💪 Key Benefits

- Consistency: Eliminates "works on my machine" problems
- Isolation: Prevents dependency conflicts with host system
- Portability: Runs identically across development, testing, and production
- Simplified Deployment: Reduces user setup to installing Docker and running a container
- Resource Management: Provides built-in tools for controlling CPU, memory, and network usage

Example `Dockerfile` implementation to package an MCP Server

```
FROM node:18-slim

WORKDIR /app

COPY package*.json ./
RUN npm install

COPY . .

EXPOSE 3000

CMD ["node", "server.js"]
```

Users run it as follows:

```
docker run -p 3000:3000 your-mcp-server-image
```

---

## MCP Server Security
  
### 🔵 Secure MCP Server Dependencies

Ensure your MCP Server is free from known vulnerabilities in third-party dependencies to meet security requirements and facilitate organizational adoption.

#### 💡 Why It Matters

MCP Servers typically require broad access and integration capabilities, making any vulnerability a significant security risk. Organizational IT and security teams scrutinize these dependencies before approving adoption.

- MCP Servers must meet stringent security and compliance requirements
- Vulnerable dependencies create potential entry points for malicious actors
- Security is mandated by SBOM requirements following the SolarWinds attack

#### ✅ Recommended Practice

- Regularly scan dependencies for known vulnerabilities
- Keep all components updated to the latest secure versions
- Monitor security advisories related to your dependencies
- Ensure compliance with licensing and security standards

